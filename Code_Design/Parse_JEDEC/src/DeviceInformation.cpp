/*
 * DeviceInformation.cpp
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */
#include <cstring>
#include <stdexcept>
#include "DeviceInformation.h"

using namespace std;

const DeviceInformation DeviceInformation::deviceInformation[] = {
      // name     name_length block_size block_num post IDCODE      IDCODE mask  mapfilename
      {"XC2C32A",       7,         260,       48,   6,  0x00000000, 0xFFFFFFFF, "xc2c32a.map" },
      {"XC2C32",        6,         260,       48,   6,  0x00000000, 0xFFFFFFFF, "xc2c32.map"  },
      {"XC2C64A",       7,         274,       96,   7,  0xF6E5F093, 0x0FFF8FFF, "xc2c64a.map" },
      {"XC2C64",        6,         274,       96,   7,  0x00000000, 0xFFFFFFFF, "xc2c64.map"  },
      {"XC2C128",       7,         752,       80,   7,  0x00000000, 0xFFFFFFFF, "xc2c128.map" },
      {"XC2C256",       7,        1364,       96,   7,  0x00000000, 0xFFFFFFFF, "xc2c256.map" },
      {"XC2C384",       7,        1868,      120,   7,  0x00000000, 0xFFFFFFFF, "xc2c384.map" },
      {"XC2C512",       7,        1980,      160,   8,  0x00000000, 0xFFFFFFFF, "xc2c512.map" },
      {"",              0,           0,        0,   0,  0x00000000, 0xFFFFFFFF, ""            },
};

/**
 * Find information for device
 *
 * @param deviceName  Name of device
 *
 * @return Pointer to static structure describing device
 */
const DeviceInformation *DeviceInformation::findDevice(const char *deviceName) {

   for (const DeviceInformation *info = deviceInformation; ; info++) {
      if (info->block_num == 0) {
         break;
      }
      if (strnicmp(deviceName, info->name, info->name_length) == 0) {
         return info;
      }
   }
   return nullptr;
}

/**
 * Find information for device
 *
 * @param idcode  IDCODE of device
 *
 * @return Pointer to static structure describing device
 */
const DeviceInformation *DeviceInformation::findDevice(uint32_t idcode) {

   for (const DeviceInformation *info = deviceInformation; ; info++) {
      if (info->block_num == 0) {
         break;
      }
      if (((info->idcode^idcode)&info->idcodeMask) == 0) {
         return info;
      }
   }
   return nullptr;
}




