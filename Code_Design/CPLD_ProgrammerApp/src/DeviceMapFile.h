/*
 * DeviceMapFile.h
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */

#ifndef DEVICEMAPFILE_H_
#define DEVICEMAPFILE_H_

#include <stdio.h>
#include "BitArray.h"
#include "JedecFile.h"
#include "DeviceInformation.h"

class DeviceMapFile {

private:

   // Number of fuses [0..fuse_limit]
   unsigned   fuse_limit = 0;
   unsigned   *fuse_map = nullptr;

   const DeviceInformation &deviceInformation;

   enum SpecialFuses {
      MAP_ZERO  = -1,  // Unused fuse should be mapped to 0
      MAP_ONE   = -2,  // Unused fuse should be mapped to 1
      MAP_SPARE = -3,  // Spare fuses
      MAP_SEC   = -10, // Security fuses, sec_0 = -10, ... , sec_6 = -4 */
      MAP_DONE  = -12, // Done fuses, done_0 = -12, done_1 = -11 */
      MAP_USER  = -44, // Uses fuses, user_0 = -44, ..., user_31 = -13 */
   };

   /**
    * Read map file from fp
    *
    * @param fp FILE to read
    *
    * @return nullptr    => success
    * @return != nullptr => error message
    */
   const char *loadFile(FILE *fp);

   unsigned block_num = 0; // Size of block (including special fuses)

public:

   /**
    * Constructor
    *
    * @param fuse_limit Number of fuses
    * @param deviceName Name of device
    */
   DeviceMapFile(const DeviceInformation &deviceInformation);

   ~DeviceMapFile() {
      if (fuse_map != nullptr) {
         free(fuse_map);
      }
   }

   /**
    * Read map file
    *
    * @param map_filename FILE to read
    *
    * @return nullptr    => success
    * @return != nullptr => error message
    */
   const char *loadFile(unsigned fuse_limit);

   void printMap();
   unsigned getSpecialFuse1(const char *fuseName);
   unsigned getSpecialFuse2(const char *fuseName);

   void jedecfile2bitfile(const JedecFile &jedecFile, BitArray &bits, uint32_t usercode=0xFFFFFFFF);

   const DeviceInformation &getDeviceInformation() {
      return deviceInformation;
   }

};

#endif /* DEVICEMAPFILE_H_ */
