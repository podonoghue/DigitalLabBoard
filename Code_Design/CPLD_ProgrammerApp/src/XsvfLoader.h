/*! \file
    \brief PC Application for XSVF player on Kinetis devices

    XsvfLoader.h

    \verbatim
    Copyright (C) 2019 Peter O'Donoghue

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

#ifndef XSVFLOADER_H_
#define XSVFLOADER_H_

#include "libusb.h"
#include "UsbCommandMessage.h"
#include "Xsvf.h"

class XsvfLoader {

public:
   XsvfLoader();
   virtual ~XsvfLoader();

private:
   XsvfLoader(const XsvfLoader &other) = delete;
   XsvfLoader(XsvfLoader &&other) = delete;
   XsvfLoader& operator=(const XsvfLoader &other) = delete;
   XsvfLoader& operator=(XsvfLoader &&other) = delete;

   libusb_context *libusbContext = nullptr;
   libusb_device_handle *deviceHandle = nullptr;

   /**
    * Locate USB device to program
    *
    * @return nullptr   => Failed
    * @return !=nullptr => LIBUSB device handle
    */
   libusb_device_handle *findDevice();

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
   const char *sendXsvfBlock(unsigned xsvf_size, uint8_t *xsvf_data);

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
   const char *sendXsvf(unsigned xsvf_size, uint8_t *xsvf_data);

public:
   /**
    * Execute XSVF sequence on device.
    * - This will open the device.
    * - Execute sequence
    * - Close the device
    *
    * @param xsvf_size
    * @param xsvf_data
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *executeXsvf(Xsvf &xsvf);

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
   const char *readIdcode(uint32_t &idcode);

   /**
    * Check target Vref is present
    * - This will open the device.
    * - Check Vdd is present on the CPLD
    * - Close the device
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *checkTargetVref();

};

#endif /* XSVFLOADER_H_ */
