/*
 * DeviceMapFile.cpp
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "DeviceMapFile.h"
#include "DeviceInformation.h"

#if (linux == 1)
/** Path to read-only data files on Linux */
static const char *LINUX_DATA_PATH = "/usr/share/digitallabboard/";
#endif

/**
 * Constructor
 *
 * @param fuse_limit Number of fuses
 * @param deviceName Name of device
 */
DeviceMapFile::DeviceMapFile(const DeviceInformation &deviceInformation) : deviceInformation(deviceInformation) {
}

/**
 * Read map file
 *
 * @param map_filename FILE to read
 *
 * @return nullptr    => success
 * @return != nullptr => error message
 */
const char *DeviceMapFile::loadFile(unsigned fuseLimit) {
   fuse_limit = fuseLimit;
   fuse_map = (unsigned*) malloc(fuse_limit);

   FILE *fp = nullptr;
#if (linux == 1)
   std::string path = std::string(LINUX_DATA_PATH).append(deviceInformation.getMapfilename());
   fp = fopen(path.c_str(), "rb");
#else
   fp = fopen(deviceInformation.getMapfilename(), "rb");
#endif
   if (fp == nullptr) {
      return "Failed to open file";
   }
   const char *msg = loadFile(fp);
   fclose(fp);

   return msg;
}

unsigned DeviceMapFile::getSpecialFuse1(const char *fuseName) {
   if (strncmp(fuseName, "sec_", 4) == 0) {
      return (DeviceMapFile::MAP_SEC+fuseName[4]-'0');
   }
   if ((strncmp(fuseName, "done<", 5) == 0) && (fuseName[6] == '>')) {
      return (DeviceMapFile::MAP_DONE+fuseName[5]-'0');
   }
   return DeviceMapFile::MAP_ONE;
}

unsigned DeviceMapFile::getSpecialFuse2(const char *fuseName) {
   if (strcmp(fuseName, "spare") == 0) {
      return (DeviceMapFile::MAP_SPARE);
   }
   if ((strncmp(fuseName, "user ", 5) == 0)) {
      fuseName += 5;
   }
   bool foundDigit = false;
   int fuseNum = 0;
   while (isdigit(*fuseName)) {
      foundDigit = true;
      fuseNum = 10*fuseNum + *fuseName++ - '0';
   }
   if (!foundDigit) {
      return MAP_ONE;
   }
   return DeviceMapFile::MAP_USER+fuseNum;
}

/**
 * Read map file from fp
 *
 * @param fp FILE to read
 *
 * @return nullptr    => success
 * @return != nullptr => error message
 */
const char *DeviceMapFile::loadFile(FILE *fp) {
   auto strtok = [](char *start=nullptr) {
      static char *ch_ptr = nullptr;
      if (start != nullptr) {
         ch_ptr = start;
      }
      assert(ch_ptr != nullptr);
      char *token = ch_ptr;
      while ((*ch_ptr != '\0') && (*ch_ptr != '\t')) {
         if ((*ch_ptr == '\n') || (*ch_ptr == '\r')) {
            *ch_ptr = '\0';
         }
         ch_ptr++;
      }
      *ch_ptr++ = '\0';
      if (token == ch_ptr) {
         return (char *)nullptr;
      }
      return token;
   };

   char     buff[1000];
   char     *suffix_ptr;
   unsigned row = 0;

   // Two extra values for security/done, spare/user
   block_num = deviceInformation.getBlockNum()+2;

   fuse_map = (unsigned int*)malloc(block_num*deviceInformation.getBlockSize()*sizeof(unsigned int));

   /*
    * Blocks are arranged in columns [0..block_num-1]
    * Block contents [0..block_size-1]
    * There are 2 special blocks on column block_num-2, block_num-1
    */
   do {
      if (fgets(buff, sizeof(buff), fp) == nullptr) {
         break;
      }
      char *token = strtok(buff);
      // Do usual blocks
      unsigned fuse_num = 0;
      for (unsigned block=0; block<(block_num-2); block++) {
         bool isEmpty = true;
         fuse_num = 0;
         while (isdigit(*token)) {
            isEmpty  = false;
            fuse_num = 10*fuse_num + (*token++ - '0');
         }
         if (isEmpty) {
            // Empty fuses map to 1
            fuse_num = MAP_ONE;
         }
         fuse_map[row*block_num+block] = fuse_num;
         //         printf("%5d ", fuse_num);
         token = strtok();
      }
      // Special block [N-2]
      suffix_ptr = token;
      if ((suffix_ptr != nullptr) && (strlen(suffix_ptr) > 0)) {
         fuse_num = getSpecialFuse1(suffix_ptr);
         //         printf("%10d   ", fuse_num);
         //         printf("'%s' ", suffix_ptr);
      }
      else {
         fuse_num = MAP_ONE;
         //         printf("%10d   ", fuse_num);
      }
      fuse_map[row*block_num+block_num-2] = fuse_num;
      // Special block [N-1]
      suffix_ptr = strtok();
      if ((suffix_ptr != nullptr) && (strlen(suffix_ptr) > 0)) {
         fuse_num = getSpecialFuse2(suffix_ptr);
         //         printf("%10d   ", fuse_num);
         //         printf("'%s'", suffix_ptr);
      }
      else{
         fuse_num = MAP_ONE;
         //         printf("%10d  ", fuse_num);
      }
      fuse_map[row*block_num+block_num-1] = fuse_num;
      //      printf("\n");
      row++;
   } while (true);

   if (!feof(fp)) {
      return "Failed to reach end of file";
   }

   return nullptr;
}

void DeviceMapFile::printMap() {
   for (unsigned row=0; row<deviceInformation.getBlockSize(); row++) {
      if ((row%100) == 0) {
         printf("     ");
         for (unsigned fuseNum=0; fuseNum<block_num; fuseNum++) {
            printf("%6d", fuseNum);
         }
         printf("\n      ");
         for (unsigned fuseNum=0; fuseNum<(6*block_num); fuseNum++) {
            printf("-");
         }
         printf("\n");
      }
      printf("%04d ", row);
      for (unsigned col=0; col<block_num; col++) {
         printf("%6d", fuse_map[col+row*block_num]);
      }
      printf("\n");
   }
}

void DeviceMapFile::jedecfile2bitfile(const JedecFile &jedecFile, BitArray &bits, uint32_t usercode){
   bits.allocateFuses(deviceInformation.getBlockSize()*block_num);
   for (unsigned block=0; block<block_num; block++) {
      for (unsigned blockIndex=0; blockIndex<deviceInformation.getBlockSize(); blockIndex++) {
         int bitnum = (block+1)*deviceInformation.getBlockSize() - blockIndex - 1;
         int fuseIndex = fuse_map[blockIndex*block_num + block];
         bool fuseValue = true;
         switch(fuseIndex) {
            case MAP_ZERO:
            case MAP_DONE+1:
               fuseValue = false;
               break;
            case MAP_ONE:
            case MAP_SPARE:
            case MAP_SEC ... MAP_SEC+6:
            case MAP_DONE:
               fuseValue = true;
               break;
            case MAP_USER ... MAP_USER+31:
            {
//               unsigned index = fuseIndex-MAP_USER;
               fuseValue = ((usercode >> (fuseIndex-MAP_USER)) & 1)?true:false;
//               printf("map[%d] = %d, %d\n", bitnum, index, fuseValue);
            }
               break;
            default:
               /* xc2c32a.map from 10.1 contain 0 .. 12278 versus 0..12277 */
               if (fuseIndex < (int)jedecFile.getFuseLimit()) {
                  fuseValue = jedecFile.getFuse(fuseIndex);
               }
         }
         bits.setFuse(bitnum, fuseValue);
      }
   }
}
