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
#include "xsvfPlayer.h"
#include "usb.h"
#include "UsbCommandMessage.h"
#include "JtagInterface.h"

// Allow access to USBDM methods
using namespace USBDM;

/**
 * Report command to console
 *
 * @param message Command message to describe
 */
static void writeCommandMessage(UsbCommandMessage &message) {
   console.WRITE(getCommandName(message.command));
   if (message.byteLength>0) {
   }
   else {
   }
   console.WRITELN();
}

using ProgrammerBusyLed = GpioD<7>;

bool execute(unsigned xsvd_size, uint8_t *xsvf_data, uint32_t &result) {

   XsvfPlayer_Array xsvf(xsvd_size, xsvf_data);
   if (xsvf.playAll()) {
      uint8_t const *idcodeBuff = xsvf.getTdoBuff();

      result = (idcodeBuff[0]<<24)+(idcodeBuff[1]<<16)+(idcodeBuff[2]<<8)+idcodeBuff[3];
      return true;
   }
   else {
      console.WRITE("Failed to execute XSVF, rc = ").WRITELN(xsvf.getError());
      return false;
   }
}

//bool readIdcodeX(uint32_t &idcode) {
//
//#define BYTES32(x) (0xFF&((x)>>24)),(0xFF&((x)>>16)),(0xFF&((x)>>8)),(0xFF&(x))
//
//   static constexpr uint8_t IDCODE_COMMAND = 0x01;
//   static constexpr uint8_t BYPASS_COMMAND = 0xFF;
//   static constexpr uint8_t ENABLE_COMMAND = 0xE8;
//   static constexpr uint8_t ERASE_COMMAND  = 0xED;
//   static constexpr uint8_t INIT_COMMAND   = 0xF0;
//   static constexpr uint8_t CONLD_COMMAND  = 0xC0;
//   static constexpr uint8_t VERIFY_COMMAND = 0xEE;
//
//   static const uint8_t xsvf_verify[] = {
//         XREPEAT,          0,           // No repeats
//         XENDIR,           false,       // Set exit to IDLE state (XSIR)
//         XENDDR,           false,       // Set exit to IDLE state (XSDR_TDO_CAPTURE)
//         XRUNTEST,         BYTES32(0),  // Disable XRUNTEST
//         XSTATE,           Reset,       // Reset target
//         XSTATE,           Idle,        // Move to IDLE state
//
//         // Confirm IDCODE
//         // IR(8)<-1, DR(32)&0x0FFF8FFF =? 0xF6E5F093 =>IDLE
//         XSIR,             8,  IDCODE_COMMAND,    // Select IDCODE register (Register #1) -> IDLE
//         XSDRSIZE,         BYTES32(32),
//         XTDOMASK,         BYTES32(0x0FFF8FFF),
//         XSDRTDO,          BYTES32(0x00000000), BYTES32(0xF6E5F093),
//
//         // Check read/write protect
//         // IR(8)<-0xFF,->01, =>IDLE
//         XSIR,             8,     BYPASS_COMMAND,  // Select BYPASS register/Get read/write protect?
//
//         // Verify
//         // IR(8)<-EE =>IR_Pause;
//         XENDIR,           true,                    // Set exit to IR_Pause state (XSIR)
//         XSIR,             8,     VERIFY_COMMAND,   // IR = 0xED, -> IR_Pause
//         XSDR,             7,     0x00,
//         XENDIR,           false,         // Set exit to IDLE state (XSIR)
//         //IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
//         XSTATE,           IR_Exit2,      // -> IR_Exit2
//         XSTATE,           IR_Update,     // -> IR_Update
//         XSTATE,           DR_Select,     // -> DR_Select
//         XSTATE,           DR_Capture,    // -> DR_Capture
//         XSTATE,           DR_Exit1,      // -> DR_Exit1
//         XSTATE,           DR_Pause,      // -> DR_Pause
//         XSTATE,           Idle,          // -> ... Idle
//         XWAIT,            Idle, Idle, BYTES32(100),
//
//         //SDR 7 TDI (70) SMASK (7f) ;
//         //RUNTEST 100 TCK;
//         //SDR 274 TDI   (03ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff)
//         //        SMASK (03ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff)
//         //        TDO   (03ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff)
//         //        MASK (/03ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff) ;
//
//   };
//
//   static const uint8_t xsvf_erase[] = {
//         XREPEAT,          0,           // No repeats
//         XENDIR,           false,       // Set exit to IDLE state (XSIR)
//         XENDDR,           false,       // Set exit to IDLE state (XSDR_TDO_CAPTURE)
//         XRUNTEST,         BYTES32(0),  // Disable XRUNTEST
//         XSTATE,           Reset,       // Reset target
//         XSTATE,           Idle,        // Move to IDLE state
//
//         // Confirm IDCODE
//         // IR(8)<-1, DR(32)&0x0FFF8FFF =? 0xF6E5F093 =>IDLE
//         XSIR,             8,  IDCODE_COMMAND,    // Select IDCODE register (Register #1) -> IDLE
//         XSDRSIZE,         BYTES32(32),
//         XTDOMASK,         BYTES32(0x0FFF8FFF),
//         XSDRTDO,          BYTES32(0x00000000), BYTES32(0xF6E5F093),
//
//         // Check read/write protect
//         // IR(8)<-0xFF,->01, =>IDLE
//         XSIR,             8,     BYPASS_COMMAND,  // Select BYPASS register/Get read/write protect?
//
//         // Enable erase
//         // IR(8)<-0xE8 => IDLE
//         XSIR,             8,     ENABLE_COMMAND,   // Select Enable
//
//         // Erase
//         // IR(8)<-ED =>IR_Pause
//         XENDIR,           true,                   // Set exit to IR_Pause state (XSIR)
//         XSIR,             8,     ERASE_COMMAND,   // IR = 0xED, -> IR_Pause
//
//         XENDIR,           false,         // Set exit to IDLE state (XSIR)
//         // Standard path: IRPAUSE-IREXIT2-IRUPDATE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
//         XSTATE,           IR_Exit2,      // -> IR_Exit2
//         XSTATE,           IR_Update,     // -> IR_Update
//         XSTATE,           DR_Select,     // -> DR_Select
//         XSTATE,           DR_Capture,    // -> DR_Capture
//         XSTATE,           DR_Exit1,      // -> DR_Exit1
//         XSTATE,           DR_Pause,      // -> DR_Pause
//
//         XWAIT,            DR_Pause, DR_Pause, BYTES32(20),
//
//         // Standard path: DRPAUSE-DREXIT2-DRUPDATE-IDLE
//         XSTATE,           DR_Exit2,      // -> DR_Exit2
//         XSTATE,           DR_Update,     // -> DR_Update
//         XSTATE,           Idle,          // -> Idle
//
//         XWAIT,            Idle, Idle, BYTES32(100000),
//
//         // Standard path:  IDLE-DRSELECT-DRCAPTURE-DREXIT1-DRPAUSE
//         XSTATE,           DR_Select,     // -> DR_Select
//         XSTATE,           DR_Capture,    // -> DR_Capture
//         XSTATE,           DR_Exit1,      // -> DR_Exit1
//         XSTATE,           DR_Pause,      // -> DR_Pause
//
//         XWAIT,            DR_Pause, DR_Pause, BYTES32(5000),
//         XWAIT,            Idle,     Idle,     BYTES32(1),
//
//         // Init
//         // IR(8)<-F0 =>IR_Pause
//         XENDIR,           true,          // Set exit to IR_Pause state (XSIR)
//         XSIR,             8,     0xF0,   // IR = 0xF0, -> IR_Pause
//         XSTATE,           IR_Exit2,      // -> IR_Exit2
//         XSTATE,           IR_Update,     // -> IR_Update
//         XSTATE,           Idle,          // -> Idle
//         XWAIT,            Idle, Idle, BYTES32(20),
//
//         // Init
//         // IR(8)<-F0 =>IR_Pause
//         XENDIR,           true,          // Set exit to IR_Pause state (XSIR)
//         XSIR,             8,     INIT_COMMAND,   // IR = 0xF0, -> IR_Pause
//         XSTATE,           IR_Exit2,      // -> IR_Exit2
//         XSTATE,           IR_Update,     // -> IR_Update
//         XSTATE,           DR_Select,     // -> DR_Select
//         XSTATE,           DR_Capture,    // -> DR_Capture
//         XSTATE,           DR_Exit1,      // -> DR_Exit1
//         XSTATE,           DR_Update,     // -> DR_Update
//         XSTATE,           Idle,          // -> Idle
//         XWAIT,            Idle, Idle, BYTES32(800),
//
//         // Conld ?
//         // IR(8)<-C0 =>Idle
//         XENDIR,           false,                     // Set exit to IDLE state (XSIR)
//         XSIR,             8,     CONLD_COMMAND,      // IR = 0xC0, -> Idle
//         XWAIT,            Idle,  Idle, BYTES32(100),
//
//         // Conld ?
//         // IR(8)<-C0 =>Idle
//         XENDIR,           false,                     // Set exit to IDLE state (XSIR)
//         XSIR,             8,     CONLD_COMMAND,      // IR = 0xC0, -> Idle
//         XWAIT,            Idle,  Idle, BYTES32(100),
//
//         XCOMPLETE,
//   };
//
//   XsvfPlayer_Array xsvf(sizeof(xsvf_erase), xsvf_erase);
//   if (xsvf.playAll()) {
//      uint8_t const *idcodeBuff = xsvf.getTdoBuff();
//
//      idcode = (idcodeBuff[0]<<24)+(idcodeBuff[1]<<16)+(idcodeBuff[2]<<8)+idcodeBuff[3];
//      return true;
//   }
//   else {
//      console.WRITE("Failed to execute XSVF sequence, rc = ").WRITELN(xsvf.getError());
//      return false;
//   }
//}

/**
 * Read IDCODE from CPLD
 *
 * @param[out] idcode  IDCODE as 32-bit integer
 *
 * @return false  => Success. IDCODE has value
 * @return true   => Failed to read IDCODE
 */
bool readIdcode(uint32_t &idcode) {

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
      console.WRITE("Failed to read IDCODE, rc = ").WRITELN(xsvf.getError());
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
         console.WRITELN(getCommandName(command.command));
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
         console.WRITELN("Programming successful");
         return true;
      }
      else {
         // Failure
         console.WRITE("Programming failed, rc = ").WRITELN(xsvf.getError());
         return false;
      }
   }

   XsvfPlayer_USB(unsigned xsvf_data_size) : XsvfPlayer(xsvf_data_size) {
   }

   ~XsvfPlayer_USB() {
   }
};

   static UsbCommandMessage command;

enum UsbState {UsbStartUp, UsbIdle, UsbWaiting};

/**
 * Poll USB interface for activity
 *
 * If active the return may be very much delayed e.g. programming of a device.
 */
void pollUsb() {

   static UsbState usbState = UsbStartUp;

   static auto cb = [](const UsbImplementation::UserEvent) {
      // Restart USB transfers on reset etc.
      usbState = UsbIdle;
      return E_NO_ERROR;
   };

   if (usbState == UsbStartUp) {
      // 1st time - Initialise hardware
      ProgrammerBusyLed::setOutput(
            PinDriveStrength_High,
            PinDriveMode_PushPull,
            PinSlewRate_Slow);

      // Start USB
      console.WRITELN("UsbStartUp");
      UsbImplementation::initialise();
      UsbImplementation::setUserCallback(cb);
      checkError();
      usbState = UsbIdle;
      return;
   }
   // Check for USB connection
   if (!UsbImplementation::isConfigured()) {
//      console.WRITELN("Not configured");
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
   // Default to Failed small response
   ResponseMessage    response;
   response.status     = UsbCommandStatus_Failed;
   response.byteLength = 0;
   unsigned responseSize = sizeof(SimpleResponseMessage);

   bool noVref = !JtagInterface::checkVref();
   if (noVref) {
      console.WRITELN("No target Vref");
   }
   do {
      if (size < (int)sizeof(command.command)) {
         // Incomplete command?
         console.WRITELN("Incomplete command");
         continue;
      }
      // Report message on console
      writeCommandMessage(command);

      switch(command.command) {
         default:
            console.WRITE("Unexpected command: ").WRITELN(command.command);
            continue;

         case UsbCommand_Nop:
            response.status = UsbCommandStatus_OK;
            continue;

         case UsbCommand_Identify:
            if (noVref) {
               continue;
            }
            if (!readIdcode(response.idcode)) {
               console.WRITELN("Failed read IDCODE");
               continue;
            }
            if (response.idcode == 0xFFFFFFFF) {
               console.WRITELN("Failed read IDCODE (no device?)");
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            response.byteLength  = 4;
            responseSize         = sizeof(ResponseIdentifyMessage);
            console.WRITE("IDCODE = 0x").WRITELN(response.idcode, Radix_16);
            continue;

         case UsbCommand_CheckVref:
            if (noVref) {
               continue;
            }
            response.status      = UsbCommandStatus_OK;
            console.WRITELN("Target Vref present");
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
            console.WRITE("Result = 0x").WRITELN(response.result, Radix_16);
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

         case UsbCommand_Status_Leds:
            ProgrammerBusyLed::write(command.failLed);
            continue;
      }
   } while (false);

   if (command.command != UsbCommand_Status_Leds) {
      ProgrammerBusyLed::write(response.status != UsbCommandStatus_OK);
   }

   Usb0::sendBulkData(responseSize, (uint8_t *)&response, 1000);
   usbState = UsbIdle;
}

int main() {

   console.WRITELN("Starting");

	for(;;) {
	pollUsb();
   }
   return 0;
}
