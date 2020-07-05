/*
 ============================================================================
 * @file    xsf_usb_player.cpp
 *
 * Stand-alone XSVF player
 * Plays a XSVF sequence hard-programmed in a C array.
 *
 *  Created on: 22 Dec 2019
 *      Author: podonoghue
 ============================================================================
 */
#include "hardware.h"
#include "Configure.h"
#include "xsvfPlayer.h"
#include "usb.h"
#include "UsbCommandMessage.h"
#include "JtagInterface.h"
#include "xsf_usb_player.h"

// Allow access to USBDM methods
using namespace USBDM;

UsbCommandMessage UsbXsvfInterface::command;

/**
 * Report command to console
 *
 * @param message Command message to describe
 */
static void writeCommandMessage(UsbCommandMessage &message) {
   console.write(getCommandName(message.command));
   if (message.byteLength>0) {
   }
   else {
   }
   console.writeln();
}

bool UsbXsvfInterface::execute(unsigned xsvd_size, uint8_t *xsvf_data, uint32_t &result) {

   XsvfPlayer_Array xsvf(xsvd_size, xsvf_data);
   if (xsvf.playAll()) {
      uint8_t const *idcodeBuff = xsvf.getTdoBuff();

      result = (idcodeBuff[0]<<24)+(idcodeBuff[1]<<16)+(idcodeBuff[2]<<8)+idcodeBuff[3];
      return true;
   }
   else {
      console.write("Failed to execute XSVF, rc = ").writeln(xsvf.getError());
      return false;
   }
}

/**
 * Read IDCODE from CPLD
 *
 * @param[out] idcode  IDCODE as 32-bit integer
 *
 * @return false  => Success. IDCODE has value
 * @return true   => Failed to read IDCODE
 */
bool UsbXsvfInterface::readIdcode(uint32_t &idcode) {

   #define BYTES32(x) (0xFF&((x)>>24)),(0xFF&((x)>>16)),(0xFF&((x)>>8)),(0xFF&(x))

   static const uint8_t idcode_message[] = {
         /* Reset           */ XREPEAT, 0,         // No repeats
         /* Reset           */ XENDIR, Ex_Idle,    // Set exit to IDLE state (XSIR etc)
         /* Reset           */ XENDDR, Ex_Idle,    // Set exit to IDLE state (XSDR_TDO etc)
         /* Reset           */ XSTATE, Reset,      // Reset JTAG
         /* Reset           */ XSTATE, Idle,       // Move to IDLE state

         // Read IDCODE
         /* Idle            */ XSIR, 8,            // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /*                 */ IDCODE_COMMAND,
         /* Idle            */ XSDR_TDOCAP, 32,    // Read IDCODE register -> TDO
         /* tdi_value    32:*/ 0x00, 0x00, 0x00, 0x00,
         /* Idle            */ XSTATE, Idle,
         /* Reset           */ XSTATE, Reset,      // Reset JTAG
         /* Reset           */ XCOMPLETE,          // End sequence
   };

   XsvfPlayer_Array xsvf(sizeof(idcode_message), idcode_message);
   if (xsvf.playAll()) {
      uint8_t const *idcodeBuff = xsvf.getTdoBuff();

      idcode = (idcodeBuff[0]<<24)+(idcodeBuff[1]<<16)+(idcodeBuff[2]<<8)+idcodeBuff[3];
      return true;
   }
   else {
      console.write("Failed to read IDCODE, rc = ").writeln(xsvf.getError());
      return false;
   }
}

class XsvfPlayer_USB : public XsvfPlayer {

private:
   //
   XsvfBlockCommandMessage command = {
         /* command;    */ UsbCommand_Nop, // Command to execute
         /* byteLength; */ 0,              // Size of data
         /* data[];     */ {},             // Data
   };

   // Count of bytes processed in current block
   unsigned blockByteCounter = 0;

   // Outstanding USB response needed
   bool doResponse = false;

   bool getBlock() {
      if (doResponse) {
         static const SimpleResponseMessage response {
            /* response.status     */ UsbCommandStatus_OK,
            /* response.byteLength */ 0,
         };
         Usb0::sendBulkData(sizeof(response), (uint8_t *)&response, 1000);
      }
      // Need to ACK future requests
      doResponse = true;

      // Reset data index
      blockByteCounter = 0;

      uint16_t size = sizeof(command);
      ErrorCode rc = Usb0::receiveBulkData(size, (uint8_t *)&command);
      if (rc != E_NO_ERROR) {
         command.byteLength = 0;
         error = "Failed to read XSVF block from host";
         return false;
      }
      if (command.command != UsbCommand_XSVF_data) {
         console.writeln(getCommandName(command.command));
         error = "Unexpected USB packet type";
         return false;
      }
      //      writeCommandMessage(*(UsbCommandMessage *)&command);
      return true;
   }

   /**
    * Get single byte from XSVF input
    *
    * @return Byte obtained
    */
   uint8_t get() {
      if (byteCounter>xsvf_data_size) {
         return XCOMPLETE;
      }
      if (blockByteCounter >= command.byteLength) {
         if (!getBlock()) {
            return XCOMPLETE;
         }
      }
      byteCounter++;
      if ((byteCounter % 1000) == 0) {
         ProgrammerBusyLed::toggle();
      }
      return command.data[blockByteCounter++];
   }

public:
   static bool doReceiveXsvf(unsigned xsvf_size) {

      XsvfPlayer_USB xsvf{xsvf_size};

      SimpleResponseMessage   response;
      response.status       = UsbCommandStatus_OK;
      response.byteLength   = 0;
      Usb0::sendBulkData(sizeof(response), (uint8_t *)&response, 1000);

      if (xsvf.playAll()) {
         // Success
         console.writeln("Programming successful");
         return true;
      }
      else {
         // Failure
         console.write("Programming failed, rc = ").writeln(xsvf.getError());
         return false;
      }
   }

   XsvfPlayer_USB(unsigned xsvf_data_size) : XsvfPlayer(xsvf_data_size) {
   }

   ~XsvfPlayer_USB() {
   }
};

/**
 * Poll USB interface for activity
 *
 * If active the return may be very much delayed e.g. programming of a device.
 */
void UsbXsvfInterface::pollUsb() {

   static UsbState usbState = UsbStartUp;

   static auto cb = [](const UsbImplementation::UserEvent) {
      // Restart USB transfers on reset etc.
      usbState = UsbIdle;
      return E_NO_ERROR;
   };

   if (usbState == UsbStartUp) {
      // Start USB
      UsbImplementation::initialise();
      UsbImplementation::setUserCallback(cb);
      checkError();
      usbState = UsbIdle;
      return;
   }
   // Check for USB connection
   if (!UsbImplementation::isConfigured()) {
      // No connection
      return;
   }

   int size;

   if (usbState != UsbWaiting) {
      // UsbIdle
      // Set up to receive a message
      Usb0::startReceiveBulkData(sizeof(command), (uint8_t *)&command);
      usbState = UsbWaiting;
      return;
   }

   // UsbWaiting

   // Check if we have received a message
   size = Usb0::pollReceiveBulkData();
   if (size < 0) {
      // No message - USB still ready
      return;
   }

   // *****************************
   // We have a message to process
   // *****************************

   ProgrammerBusyLed::on();
   ProgrammerOkLed::off();

   // Default to Failed small response
   ResponseMessage    response;
   response.status     = UsbCommandStatus_Failed;
   response.byteLength = 0;
   unsigned responseSize = sizeof(SimpleResponseMessage);

   bool noVref = !JtagInterface::checkVref();
   if (noVref) {
      console.writeln("No target Vref");
   }
   do {
      if (size < (int)sizeof(command.command)) {
         // Empty message?
         console.writeln("Empty command");
         continue;
      }
      writeCommandMessage(command);

      switch(command.command) {
         default:
            console.write("Unexpected command: ").writeln(command.command);
            continue;

         case UsbCommand_Nop:
            response.status = UsbCommandStatus_OK;
            continue;

         case UsbCommand_Identify:
            if (noVref) {
               continue;
            }
            if (!readIdcode(response.idcode)) {
               console.writeln("Failed read IDCODE");
               continue;
            }
            if (response.idcode == 0xFFFFFFFF) {
               console.writeln("Failed read IDCODE (no device?)");
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            response.byteLength  = 4;
            responseSize         = sizeof(ResponseIdentifyMessage);
            console.write("IDCODE = 0x").writeln(response.idcode, Radix_16);
            continue;

         case UsbCommand_CheckVref:
            if (noVref) {
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            console.writeln("Target Vref present");
            continue;

         case UsbCommand_XSVF_execute:
            if (noVref) {
               continue;
            }
            if (!execute(command.byteLength, command.data, response.result)) {
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            response.byteLength  = 4;
            responseSize         = sizeof(ResponseIdentifyMessage);
            console.write("Result = 0x").writeln(response.result, Radix_16);
            continue;

         case UsbCommand_XSVF:
            if (noVref) {
               continue;
            }
            if (!XsvfPlayer_USB::doReceiveXsvf(command.xsvfSize)) {
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            continue;
      }
   } while (false);

   ProgrammerBusyLed::write(response.status != UsbCommandStatus_OK);
   ProgrammerOkLed::write(response.status == UsbCommandStatus_OK);

   Usb0::sendBulkData(responseSize, (uint8_t *)&response, 1000);
   usbState = UsbIdle;
}
