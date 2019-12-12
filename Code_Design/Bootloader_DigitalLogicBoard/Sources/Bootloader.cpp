/*
 * Bootloader.cpp
 *
 *  Created on: 7 Dec 2019
 *      Author: podonoghue
 */
#include "hardware.h"
#include "usb.h"
#include "flash.h"
#include "crc.h"
#include "CommandMessage.h"

using namespace USBDM;

/*
 * ICP CONTROL
 */
/// Power - Enable button - also ICP button during power-on
using IcpButton = GpioA<4,  USBDM::ActiveLow>;
using DebugPin  = GpioD<4,  USBDM::ActiveLow>;

/**
 * Report command to console
 *
 * @param message COmmand message to describe
 */
void writeCommandMessage(CommandMessage &message) {
   console.
   write(getCommandName(message.command));
   if (message.byteLength>0) {
      console.
      write(" [0x").write(message.startAddress, Radix_16).
      write("..0x").write(message.startAddress+message.byteLength-1, Radix_16).
      writeln("]");
      //   console.writeArray(message.data, message.byteLength, message.startAddress);
   }
   else {
      console.writeln();
   }
}

/** Start of Flash region being used for image */
static constexpr unsigned FLASH_BUFFER_START =  0x8000;

/** Size of Flash region being used for image */
static constexpr unsigned FLASH_BUFFER_SIZE  = 0x18000;

uint32_t calcuateCRC32(uint8_t *start, uint32_t size) {
   using Crc = Crc0;

   // Calculate CRC32
   Crc::configure_Crc32();
   uint32_t crc = Crc::calculateCrc(start, size);
   return crc;
}

/**
 * Check is flash region has valid checksum
 *
 * @return true  => CRC  is valid i.e. Flash image is verified
 * @return false => CRC  is invalid i.e. Flash image is not verified
 */
bool isFlashValid() {
   uint32_t flashCrc = *((uint32_t *)(FLASH_BUFFER_START+FLASH_BUFFER_SIZE-4));
   uint32_t calculatedCrc = calcuateCRC32((uint8_t *)FLASH_BUFFER_START, FLASH_BUFFER_SIZE-4);
   return calculatedCrc == flashCrc;
}

/**
 * Program a region of the flash image
 *
 * @param command Command containing information about region to program
 *
 * @return true  => Successfully programmed
 * @return false => Programming failed
 */
bool programFlash(CommandMessage command) {
   if (command.startAddress < FLASH_BUFFER_START) {
      return false;
   }
   if ((command.startAddress+command.byteLength) > (FLASH_BUFFER_START+FLASH_BUFFER_SIZE)) {
      return false;
   }
   FlashDriverError_t rc;
   rc = Flash::programRange(command.data, (uint8_t *)(command.startAddress), command.byteLength);
   if (rc != FLASH_ERR_OK) {
      return false;
   }
   return true;
}

void toggleDebug() {
   DebugPin::setOutput();
   for (int i=0; i<100; i++) {
      DebugPin::toggle();
      waitUS(100);
   }
}

void checkICP() {
   IcpButton::setInput(PinPull_Up);
//   toggleDebug();

   if (isFlashValid() && IcpButton::isReleased()) {
//      console.writeln("Flash is valid - calling flash image via relocated vector table");

      // Find the reset code */
      void (*f)() = (void (*)())(((uint32_t *)FLASH_BUFFER_START)[1]);

      // Call it
//      console.write("Calling startup @0x").writeln((uint32_t*)f);
      f();
   }
}

#ifndef SCB_AIRCR_VECTKEY
#define SCB_AIRCR_VECTKEY(x) (((x)<<SCB_AIRCR_VECTKEY_Pos)&SCB_AIRCR_VECTKEY_Msk)
#endif

void resetSystem() {

   /* Request system reset */
   SCB->AIRCR = SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_Msk;

   /* Wait until reset */
  for(;;) {
     __asm__("nop");
  }
}

int main() {

//   console.write("Flash image is ").writeln(isFlashValid()?"valid":"invalid");
//   console.writeln("Starting USB boot-loader");

   // Start USB
   UsbImplementation::initialise();
   checkError();

   for(;;) {
      // Wait for USB connection
      while(!UsbImplementation::isConfigured()) {
         __WFI();
      }
      CommandMessage  command;
      ResponseMessage response;
      for(;;) {
         uint16_t size = sizeof(command);
         ErrorCode rc = Usb0::receiveBulkData(size, (uint8_t *)&command);
         if (rc != E_NO_ERROR) {
            continue;
         }
//         writeCommandMessage(command);

         // Default to OK small response
         unsigned responseSize = sizeof(ResponseStatus);
         response.byteLength = 0;
         response.status     = Status_OK;

         switch(command.command) {
            default:
            case Command_Nop:
               break;

            case Command_Identify:
               response.hardwareVersion   = HW_LOGIC_BOARD_V2;
               response.bootloaderVersion = BOOTLOADER_V1;
               response.flashStart        = FLASH_BUFFER_START;
               response.flashSize         = FLASH_BUFFER_SIZE;
               responseSize               = command.byteLength + sizeof(ResponseIdentify);
               break;

            case Command_EraseFlash:
               if (Flash::eraseRange((uint8_t *)FLASH_BUFFER_START, FLASH_BUFFER_SIZE) != FLASH_ERR_OK) {
                  response.status = Status_Failed;
               }
               break;

            case Command_ProgramBlock:
               if (!programFlash(command)) {
                  response.status = Status_Failed;
//                  console.writeln("Flash programming failed");
               }
//               if (isFlashValid()) {
//                  console.writeln("Flash is valid");
//               }
               break;

            case Command_ReadBlock:
               response.status      = Status_OK;
               response.byteLength  = command.byteLength;
               responseSize         = command.byteLength + sizeof(ResponseStatus);
               memcpy(response.data, (uint8_t *)(command.startAddress), command.byteLength);
               break;

            case Command_Reset:
               resetSystem();
               break;
         }
         Usb0::sendBulkData(responseSize, (uint8_t *)&response, 1000);
      }
   }
   return 0;
}
