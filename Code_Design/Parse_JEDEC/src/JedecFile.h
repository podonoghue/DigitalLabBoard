/** ============================================================================
 * JedecFile.h
 *
 *  Created on: 29 Dec 2019
 *      Author: podonoghue
 *  ============================================================================ */
#ifndef JEDECFILE_H_
#define JEDECFILE_H_

#include <cstdint>
#include <memory>
#include <cstdio>
#include "BitArray.h"

class DeviceMapFile;

class JedecFile {

private:

   static constexpr char STX = 0x02;
   static constexpr char ETX = 0x03;

   enum State {
      S_HEADER,
      S_COMMENT,
      S_COMMENT_device,
      S_COMMENT_version,
      S_COMMAND,
      S_QCOMMAND,
      S_QF,
      S_QP,
      S_QV,
      S_FUSEDEFAULT,
      S_X,
      S_J_arch,
      S_J_pin,
      S_L_num,
      S_L_value,
      S_CHECKSUM,
   };

   // Number of fuses [0..fuse_limit]
   unsigned   fuse_limit         = 0;

   // Number of pins on device
   unsigned   number_of_pins     = 0;

   // Longest test vector
   unsigned   vector_limit       = 0;

   // Default value for fuses
   bool       default_state      = 0;

   // Default value for test vector
   bool       default_test       = 0;

   // Code indicating device architecture
   unsigned   device_code_arch   = 0;

   // Code indicating device pin-out?
   unsigned   device_code_pin    = 0;

   // Version information
   const char *version_info      = nullptr;

   // Device name
   const char *device_name       = nullptr;

   // Fuse data bit-map
   BitArray    fuse_data;

   // Fuse data re-arranged for programming
   BitArray    programming_data;

   // Checksum for file
   uint16_t    check_sum         = 0;

   /**
    * Read JEDEC file from fp
    *
    * @param fp File to read
    *
    * @return nullptr    => success
    * @return != nullptr => error message
    */
   const char *loadFile(std::FILE *fp);

   /**
    * Print fuse map
    */
   void jedecPrintFuses();

   /**
    * Calculate checksum
    *
    * @return checksum
    */
   uint16_t calculateChecksum() const {
      return fuse_data.calculateJedecChecksum();
   }

public:

   JedecFile() {
   }

   ~JedecFile() {
      if (version_info != nullptr) {
         std::free((void*)version_info);
      }
      if (device_name != nullptr) {
         std::free((void*)device_name);
      }
   }

   /**
    * Read JEDEC file
    *
    * @param jedec_filepath File to read
    *
    * @return nullptr    => success
    * @return != nullptr => error message
    */
   const char *loadFile(const char *jedec_filepath);

   /**
    * Report file contents
    */
   void report();

   /**
    * Report Programming data
    */
   void printProgrammingData(unsigned max = 0);

   /**
    * Print fuse map
    */
   void jedecPrint();

   /**
    * Get default fuse value
    *
    * @return
    */
   bool getDefaultState() const {
      return default_state;
   }

   /**
    * Get default test vector value
    *
    * @return
    */
   bool getDefaultTest() const {
      return default_test;
   }

   /**
    * Get device code
    */
   unsigned getDeviceCodeArch() const  {
      return device_code_arch;
   }

   /**
    * Get device code
    */
   unsigned getDeviceCodePin() const  {
      return device_code_pin;
   }

   /**
    * Get device name
    *
    * @return
    */
   const char* getDeviceName() const  {
      return device_name;
   }

   /**
    * Get version information
    *
    * @return
    */
   const char* getVersionName() const  {
      return version_info;
   }

   /**
    * Get fuse number limit
    */
   unsigned getFuseLimit() const  {
      return fuse_limit;
   }

   /**
    * Get number of pins
    */
   unsigned getNumberOfPins() const  {
      return number_of_pins;
   }

   /**
    * Get maximum vector length ?
    */
   unsigned getVectorLimit() const  {
      return vector_limit;
   }

   /**
    * Get checksum
    */
   uint32_t getChecksum() const  {
      return check_sum;
   }

   /**
    * Get fuse value
    *
    * @param fuseNumber Fuse number
    *
    * @return Fuse value
    */
   bool getFuse(unsigned fuseNumber) const {
      return fuse_data.getFuse(fuseNumber);
   }

   /**
    * Get programming bit value
    *
    * @param fuseNumber Bit number
    *
    * @return Fuse value
    */
   bool getProgramBit(unsigned bitNumber) const {
      return programming_data.getFuse(bitNumber);
   }

   /**
    * Get size of program data
    */
   unsigned getProgramSize() const {
      return programming_data.getFuseLimit();
   }

   /**
    * Create programming data from fuse data using mapping file
    *
    * @param mapFile  File used to map JEDEC fuses to programming bit map
    */
   void rearrangeProgrammingData(DeviceMapFile &mapFile);
};

#endif /* JEDECFILE_H_ */
