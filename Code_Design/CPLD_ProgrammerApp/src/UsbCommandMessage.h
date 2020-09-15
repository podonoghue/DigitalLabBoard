/*
 * UsbCommandMessage.h
 *
 *  Created on: 9 Dec 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_USBCOMMANDMESSAGE_H_
#define SOURCES_USBCOMMANDMESSAGE_H_

#include <stdint.h>

static constexpr uint16_t HW_LOGIC_BOARD_V2 = 1;
static constexpr uint16_t HW_LOGIC_BOARD_V3 = 2;
static constexpr uint16_t HW_LOGIC_BOARD_V4 = 3;
static constexpr uint16_t BOOTLOADER_V1     = 1;

/**
 * Commands available
 */
enum UsbCommand : uint32_t {
   UsbCommand_Nop,            //! No operation
   UsbCommand_Identify,       //! Read IDCODE
   UsbCommand_CheckVref,      //! Check for target Vref
   UsbCommand_XSVF,           //! Start XSVF download
   UsbCommand_XSVF_data,      //! Transfer XSVF block
};

/**
 * Result of command
 */
enum UsbCommandStatus : uint32_t {
   UsbCommandStatus_OK,          //!< OK result
   UsbCommandStatus_Failed,      //!< Failed
};

static constexpr unsigned MAX_MESSAGE_DATA = 1024;

/**
 * Get command name as string
 *
 * @param command
 *
 * @return Name as string
 *
 * @note return value is a pointer to a STATIC object - do not free
 */
static inline const char *getCommandName(UsbCommand command) {
   static const char *names[] = {
         "UsbCommand_Nop",
         "Command_Identify",
         "UsbCommand_CheckVref",
         "UsbCommand_XSVF",
         "UsbCommand_XSVF_data",
   };
   const char *name = "Unknown";
   if (command < (sizeof(names)/sizeof(names[0]))) {
      name = names[command];
   }
   return name;
}

#pragma pack(push, 1)

/**
 * Simple USB command message
 */
struct SimpleCommandMessage {
   UsbCommandStatus   status;        // Status
   uint32_t           byteLength;    // Size of data
};

/**
 * XSVF block USB command message
 */
struct XsvfBlockCommandMessage {
   UsbCommand         command;       // Status
   uint32_t           byteLength;    // Size of data
   uint8_t            data[1024];    // Data
};

/**
 * General USB command message
 */
struct UsbCommandMessage {
   UsbCommand  command;       // Command to execute
   uint32_t    byteLength;    // Size of data
   union {
      struct {
         uint32_t xsvfSize;
      };
      uint8_t     data[MAX_MESSAGE_DATA];    // Data
   };
};

/**
 * General USB response message
 */
struct ResponseMessage {
   UsbCommandStatus   status;        // Status
   uint32_t           byteLength;    // Size of data
   union {
      struct {
         uint32_t idcode;
      };
   };
};

/**
 * General USB response message
 */
struct SimpleResponseMessage {
   UsbCommandStatus   status;        // Status
   uint32_t           byteLength;    // Size of data
};

//===================================================

/**
 * Identify command message
 */
struct UsbCommandIdentifyMessage {
   UsbCommand  command;       // Command to execute
   uint32_t    byteLength;    // Size of data
};

/**
 * Identify response message
 */
struct ResponseIdentifyMessage {
   UsbCommandStatus   status;        // Status
   uint32_t           byteLength;    // Size of data
   uint32_t           idcode;        // IDCODE from target
};

//===================================================

/**
 * Start XSVF transfer message
 */
struct UsbStartXsvfMessage {
   UsbCommand  command;       // Command to execute
   uint32_t    byteLength;    // Size of data
   uint32_t    xsvfSize;
};

//===================================================

/**
 * Send XSVF block message
 */
struct UsbSendXsvfBlockMessage {
   UsbCommand  command;       // Command to execute
   uint32_t    byteLength;    // Size of data
   uint8_t     data[MAX_MESSAGE_DATA];    // Data
};

//===================================================

#pragma pack(pop)

#endif /* SOURCES_USBCOMMANDMESSAGE_H_ */
