/*
 * Bootloader.h
 *
 *  Created on: 9 Dec 2019
 *      Author: podonoghue
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "FlashImage.h"
#include "libusb.h"
#include "UsbCommandMessage.h"
#include "crc32b.h"

class Bootloader {

public:
   Bootloader();
   virtual ~Bootloader();

   /**
    * Program a flash image to device
    *
    * @param flashImage    Flash image being programmed to device
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *download(FlashImagePtr flashImage);

   /**
    * Get information about the device
    *
    * @param[out] identity
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *getDeviceInformation(ResponseIdentify &identity);

private:
   Bootloader(const Bootloader &other) = delete;
   Bootloader(Bootloader &&other) = delete;
   Bootloader& operator=(const Bootloader &other) = delete;
   Bootloader& operator=(Bootloader &&other) = delete;

   libusb_context *libusbContext = nullptr;
   libusb_device_handle *deviceHandle = nullptr;
   uint32_t flashStart           = 0;
   uint32_t flashSize            = 0;
   uint16_t hardwareVersion      = 0;
   uint16_t bootloaderVersion    = 0;
   uint32_t existingImageVersion = 0;

   Crc32 crc32;

   /**
    * Locate USB device to program
    *
    * @return nullptr   => failed
    * @return !=nullptr => LIBUSB device handle
    */
   libusb_device_handle *findDevice();

   /**
    * Reset device
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *resetDevice();

   /**
    * Program a block to device
    *
    * @param message Information to send to device
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *programBlock(UsbCommandMessage &message);

   /**
    * Erase device flash
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *eraseFlash();

   /**
    * Get information about the device
    * Assumes device has been opened
    *
    * @param[out] identity
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *queryDeviceInformation(ResponseIdentify &identity);

   /**
    * Program image to device
    *
    * @param flashImage    Flash image being programmed to device
    *
    * @return nullptr   => success
    * @return !=nullptr => failed, error message
    */
   const char *programFlash(FlashImagePtr flashImage);
};

#endif /* BOOTLOADER_H_ */
