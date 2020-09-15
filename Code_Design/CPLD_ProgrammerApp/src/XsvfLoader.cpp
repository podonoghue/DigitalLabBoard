/*! \file
    \brief PC Application for XSVF player on Kinetis devices

    XsvfLoader.cpp

    \verbatim
    Copyright (C) 2020 Peter O'Donoghue

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    \endverbatim

    \verbatim
   Change History
   -=========================================================================================
   | 10 Dec 2019 | Initial work                                              - pgo
   +=========================================================================================
   \endverbatim
 */
#include <stdio.h>

#include "libusb.h"
#include "XsvfLoader.h"
#include "UsbCommandMessage.h"

static constexpr uint8_t   EP_OUT      = LIBUSB_ENDPOINT_OUT|1;  // EP #1 OUT
static constexpr uint8_t   EP_IN       = LIBUSB_ENDPOINT_IN |2;  // EP #2 IN

static constexpr uint16_t  VENDOR_ID   = 0x16D0;    // Vendor (actually MCS)
static constexpr uint16_t  PRODUCT_ID  = 0x4331;    // Product ID

XsvfLoader::XsvfLoader() {
}

XsvfLoader::~XsvfLoader() {
}

/**
 * Locate USB device to program
 * Assumes libusb has been initialised.
 *
 * @return nullptr   => failed
 * @return !=nullptr => LIBUSB device handle
 */
libusb_device_handle *XsvfLoader::findDevice() {

   libusb_device_handle *device = nullptr;
   libusb_device **deviceList = nullptr;
   ssize_t deviceCount = 0;
   int rc = 0;

   do {
      deviceCount = libusb_get_device_list(libusbContext, &deviceList);
      if (deviceCount <= 0){
         fprintf(stderr, "No suitable devices found\n");
         continue;
      }
      // Reserve device
      device = libusb_open_device_with_vid_pid(libusbContext, VENDOR_ID, PRODUCT_ID);
      if (device == nullptr) {
         fprintf(stderr, "Device failed to open\n");
         continue;
      }
      // Find out if kernel driver is attached
      if(libusb_kernel_driver_active(device, 0) == 1) {
         fprintf(stderr, "Kernel Driver Active\n");
         // Detach it
         if(libusb_detach_kernel_driver(device, 0) == 0) {
            fprintf(stderr, "Kernel Driver Detached!\n");
         }
      }
      rc = libusb_set_configuration(device, 1);
      if (rc < 0) {
         fprintf(stderr, "Set configuration failed - %s\n", libusb_error_name(rc));
         continue;
      }
      rc = libusb_claim_interface(device, 0);
      if (rc < 0) {
         fprintf(stderr, "Claim interface failed - %s\n", libusb_error_name(rc));
         continue;
      }
   } while (false);

   // We can now free the list
   libusb_free_device_list(deviceList, true);

   if (rc < 0) {
      libusb_close(device);
      device = nullptr;
   }
   return device;
}

/**
 * Check target Vref is present
 * - This will open the device.
 * - Check Vdd is present on the CPLD
 * - Close the device
 *
 * @return nullptr   => success
 * @return !=nullptr => failed, error message
 */
const char *XsvfLoader::checkTargetVref() {
   const char *errorMessage = nullptr;

   int rc = libusb_init(&libusbContext);
   if (rc < 0) {
      return "sendSxvfFile - Libusb failed initialisation";
   }
   do {
      deviceHandle = findDevice();
      if (deviceHandle == nullptr) {
         errorMessage = "Programmer not found";
         break;
      }

      UsbCommandIdentifyMessage  command = {
            /* command      */ UsbCommand_CheckVref,
            /* byteLength   */ 0,
      };

      int bytesSent = 0;
      int rc = libusb_bulk_transfer(deviceHandle, EP_OUT, (uint8_t*)&command, sizeof(command), &bytesSent, 1000);
      if (rc < 0) {
         errorMessage = libusb_error_name(rc);
         break;
      }
      if ((unsigned)bytesSent != sizeof(command)) {
         errorMessage = "checkTargetVref - Incomplete transmission";
         break;
      }

      SimpleResponseMessage response = {};

      int bytesReceived = 0;
      rc = libusb_bulk_transfer(deviceHandle, EP_IN, (uint8_t*)&response, sizeof(response), &bytesReceived, 1000);
      if (rc < 0) {
         errorMessage = libusb_error_name(rc);
         break;
      }
      if ((unsigned)bytesReceived < sizeof(response)) {
         errorMessage = "checkTargetVref - Incomplete reception";
         break;
      }
      if (response.status != UsbCommandStatus_OK) {
         errorMessage = "No Vdd on device";
         break;
      }

   } while(false);

   if (deviceHandle != nullptr) {
      libusb_close(deviceHandle);
      deviceHandle = nullptr;
   }

   libusb_exit(libusbContext);
   return errorMessage;
}

/**
 * Read IDCODE from device
 * - This will open the device.
 * - Read IDCODE from CPLD
 * - Close the device
 *
 * @param idcode  IDCODE read from target
 *
 * @return nullptr   => success
 * @return !=nullptr => failed, error message
 */
const char *XsvfLoader::readIdcode(uint32_t &idcode) {
   const char *errorMessage = nullptr;

   int rc = libusb_init(&libusbContext);
   if (rc < 0) {
      return "readIdcode - Libusb failed initialisation";
   }
   do {
      deviceHandle = findDevice();
      if (deviceHandle == nullptr) {
         errorMessage = "Programmer not found";
         break;
      }

      UsbCommandIdentifyMessage  command = {
            /* command      */ UsbCommand_Identify,
            /* byteLength   */ 0,
      };

      int bytesSent = 0;
      int rc = libusb_bulk_transfer(deviceHandle, EP_OUT, (uint8_t*)&command, sizeof(command), &bytesSent, 1000);
      if (rc < 0) {
         errorMessage = libusb_error_name(rc);
         break;
      }
      if ((unsigned)bytesSent != sizeof(command)) {
         errorMessage = "readIdcode - Incomplete transmission";
         break;
      }

      ResponseIdentifyMessage response = {};

      int bytesReceived = 0;
      rc = libusb_bulk_transfer(deviceHandle, EP_IN, (uint8_t*)&response, sizeof(response), &bytesReceived, 1000);
      if (rc < 0) {
         errorMessage = libusb_error_name(rc);
         break;
      }
      if ((unsigned)bytesReceived < sizeof(response)) {
         errorMessage = "readIdcode - Incomplete reception";
         break;
      }
      if (response.status != UsbCommandStatus_OK) {
         errorMessage = "readIdcode - Operation failed on device";
         break;
      }
      fprintf(stderr, "IDCODE = 0x%08X\n", response.idcode);
      fflush(stderr);
      idcode = response.idcode;

   } while(false);

   if (deviceHandle != nullptr) {
      libusb_close(deviceHandle);
      deviceHandle = nullptr;
   }

   libusb_exit(libusbContext);
   return errorMessage;
}

///**
// * Execute short XSVF block (debug tests)
// *
// * @param result  Result read from target
// *
// * @return nullptr   => success
// * @return !=nullptr => failed, error message
// */
//const char *XsvfLoader::executeXsvf(uint32_t &result) {
//   const char *errorMessage = nullptr;
//
//   int rc = libusb_init(&libusbContext);
//   if (rc < 0) {
//      return "sendSxvfFile - Libusb failed initialisation";
//   }
//   do {
//      deviceHandle = findDevice();
//      if (deviceHandle == nullptr) {
//         errorMessage = "Programmer not found";
//         break;
//      }
//
//      UsbCommandIdentifyMessage  command = {
//            /* command      */ UsbCommand_Identify,
//            /* byteLength   */ 0,
//      };
//
//      int bytesSent = 0;
//      int rc = libusb_bulk_transfer(deviceHandle, EP_OUT, (uint8_t*)&command, sizeof(command), &bytesSent, 1000);
//      if (rc < 0) {
//         errorMessage = libusb_error_name(rc);
//         break;
//      }
//      if ((unsigned)bytesSent != sizeof(command)) {
//         errorMessage = "readIdcode - Incomplete transmission";
//         break;
//      }
//
//      ResponseIdentifyMessage response = {};
//
//      int bytesReceived = 0;
//      rc = libusb_bulk_transfer(deviceHandle, EP_IN, (uint8_t*)&response, sizeof(response), &bytesReceived, 1000);
//      if (rc < 0) {
//         errorMessage = libusb_error_name(rc);
//         break;
//      }
//      if ((unsigned)bytesReceived < sizeof(response)) {
//         errorMessage = "readIdcode - Incomplete reception";
//         break;
//      }
//      if (response.status != UsbCommandStatus_OK) {
//         errorMessage = "readIdcode - Operation failed on device";
//         break;
//      }
//      fprintf(stderr, "IDCODE = 0x%08X\n", response.idcode);
//      fflush(stderr);
//      result = response.idcode;
//
//   } while(false);
//
//   if (deviceHandle != nullptr) {
//      libusb_close(deviceHandle);
//      deviceHandle = nullptr;
//   }
//
//   libusb_exit(libusbContext);
//   return errorMessage;
//}

/**
 * Send one block of XSVF data for execution on opened device
 * This is one block in a sequence.
 * XSVF commands may straddle block boundaries.
 *
 * @param message Information to send to device
 *
 * @return nullptr   => success
 * @return !=nullptr => failed, error message
 */
const char *XsvfLoader::sendXsvfBlock(unsigned xsvf_size, uint8_t *xsvf_data) {
   int rc = 0;

   UsbSendXsvfBlockMessage message = {
         /* command      */ UsbCommand_XSVF_data,
         /* byteLength   */ xsvf_size,
         /* data         */ {},
   };
   for (unsigned index=0; index<xsvf_size; index++) {
      message.data[index] = xsvf_data[index];
   }
   int bytesSent   = 0;
   int bytesToSend = sizeof(SimpleCommandMessage)+message.byteLength;
   rc = libusb_bulk_transfer(deviceHandle, EP_OUT, (uint8_t*)&message, bytesToSend, &bytesSent, 1000);
   if (rc < 0) {
      return libusb_error_name(rc);
   }
   if (bytesSent != bytesToSend) {
      return "downloadXsvfBlock - Incomplete transmission";
   }
   SimpleResponseMessage response = {};

   int bytesReceived = 0;
   rc = libusb_bulk_transfer(deviceHandle, EP_IN, (uint8_t*)&response, sizeof(response), &bytesReceived, 1000);
   if (rc < 0) {
      return libusb_error_name(rc);
   }
   if ((unsigned)bytesReceived < sizeof(response)) {
      fprintf(stderr, "downloadXsvfBlock - bytesReceived = %d\n", bytesReceived);
      return "downloadXsvfBlock - Incomplete reception";
   }
   if (response.status != UsbCommandStatus_OK) {
      return "downloadXsvfBlock - Operation failed on device";
   }
   return nullptr;
}

/**
 * Send XSVF data for execution on opened device
 * This will be an entire XSVF sequence.
 *
 * @param xsvf_size
 * @param xsvf_data
 *
 * @return nullptr   => success
 * @return !=nullptr => failed, error message
 */
const char *XsvfLoader::sendXsvf(unsigned xsvf_size, uint8_t *xsvf_data) {

   // OK for empty image
   if (xsvf_size==0) {
      return nullptr;
   }

   fprintf(stderr, "sendXsvf - Sending header for SXVF Data (%d bytes)]\n", xsvf_size);

   UsbStartXsvfMessage command = {
         /* command      */ UsbCommand_XSVF,
         /* byteLength   */ (uint32_t)sizeof(command.xsvfSize),
         /* xsvfSize     */ (uint32_t)xsvf_size,
   };

   int bytesSent = 0;
   int rc = libusb_bulk_transfer(deviceHandle, EP_OUT, (uint8_t*)&command, sizeof(command), &bytesSent, 1000);
   if (rc < 0) {
      return libusb_error_name(rc);
   }
   if ((unsigned)bytesSent != sizeof(command)) {
      return "sendXsvf - Incomplete transmission";
   }

   SimpleResponseMessage response = {};

   int bytesReceived = 0;
   rc = libusb_bulk_transfer(deviceHandle, EP_IN, (uint8_t*)&response, sizeof(response), &bytesReceived, 1000);
   if (rc < 0) {
      return libusb_error_name(rc);
   }
   if ((unsigned)bytesReceived < sizeof(response)) {
      return "sendXsvf - Incomplete reception";
   }
   if (response.status != UsbCommandStatus_OK) {
      return "sendXsvf - Operation failed on device";
   }

   // Maximum size of data to write
   unsigned int maxSplitBlockSize = sizeof(UsbSendXsvfBlockMessage::data);

   const char *errrorMessage  = nullptr;
   uint32_t offset            = 0;
   uint32_t bytesToProgram    = xsvf_size;

   while (bytesToProgram>0) {
      uint32_t splitBlockSize = bytesToProgram;
      if (splitBlockSize>maxSplitBlockSize) {
         splitBlockSize = maxSplitBlockSize;
      }
      fprintf(stderr, "sendXsvf - Sending splitBlock [0x%06X..0x%06X]\n",
            offset, offset+maxSplitBlockSize-1);
      fflush(stderr);

      errrorMessage = sendXsvfBlock(splitBlockSize, xsvf_data+offset);
      if (errrorMessage != nullptr) {
         break;
      }
      offset          += splitBlockSize;
      bytesToProgram  -= splitBlockSize;
   }
   fprintf(stderr, "sendXsvf - Sent %d bytes\n", offset);
   fflush(stderr);

   return errrorMessage;
}

/**
 * Execute XSVF sequence on device.
 * - This will open the device.
 * - Execute sequence
 * - Close the device
 *
 * @param xsvf The XSVF sequence to execute
 *
 * @return nullptr   => success
 * @return !=nullptr => failed, error message
 */
const char *XsvfLoader::executeXsvf(Xsvf &xsvf) {
   const char *errorMessage = nullptr;

   int rc = libusb_init(&libusbContext);
   if (rc < 0) {
      return "sendSxvfFile - Libusb failed initialisation";
   }
   do {
      deviceHandle = findDevice();
      if (deviceHandle == nullptr) {
         errorMessage = "Programmer not found";
         break;
      }
      errorMessage = sendXsvf(xsvf.getSize(), xsvf.getData());
      if (errorMessage != nullptr) {
         break;
      }
   } while(false);

   if (deviceHandle != nullptr) {
      libusb_close(deviceHandle);
      deviceHandle = nullptr;
   }

   libusb_exit(libusbContext);

   return errorMessage;
}

