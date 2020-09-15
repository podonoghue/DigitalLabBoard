/*
 * BitArray.h
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cstring>

class BitArray {

private:
   std::uint8_t *fuse_data  = nullptr;
   unsigned      fuse_limit = 0;

public:
   BitArray() {
   }

   virtual ~BitArray() {
      if (fuse_data != nullptr) {
         free((void *)fuse_data);
      }
   }

   /**
    * Get fuse number limit
    */
   unsigned getFuseLimit() const {
      return fuse_limit;
   }

   /**
    * Allocate fuse map
    *
    * @param numberOfFuses
    */
   void allocateFuses(unsigned fuseLimit) {
      fuse_limit = fuseLimit;
      fuse_data = (uint8_t *) std::malloc((fuse_limit+7)/8);
      clearFuses(false);
   }

   /**
    * Clear fuse map to given value
    *
    * @param defaultValue
    */
   void clearFuses(bool defaultValue = false) {
      assert(fuse_data != nullptr);
      std::memset(fuse_data, defaultValue, (fuse_limit+7)/8);
   }

   /**
    * Set fuse to value
    *
    * @param fuseNumber Fuse to change
    * @param value      New value for fuse
    */
   void setFuse(unsigned fuseNumber, bool value) {
      assert(fuse_data != nullptr);
      const uint8_t mask = (1<<(fuseNumber%8));
      if (value) {
         fuse_data[fuseNumber/8] |= mask;
      }
      else {
         fuse_data[fuseNumber/8] &= ~mask;
      }
   }

   /**
    * Get fuse value
    *
    * @param fuseNumber Fuse to check
    *
    * @return Fuse value
    */
   bool getFuse(unsigned fuseNumber) const {
      const uint8_t mask = (1<<(fuseNumber%8));
      return (fuse_data[fuseNumber/8] & mask)?true:false;
   }

   /**
    * Calculate checksum
    *
    * @return checksum
    */
   uint16_t calculateJedecChecksum() const {
      uint16_t checksum = 0;
      uint8_t  mask     = 0xFF;
      for (unsigned index = 0; index<((fuse_limit+7)/8); index++) {
         if (index == ((fuse_limit+7)/8) - 1) {
            mask = fuse_limit%8;
            mask = (1<<mask);
            mask = mask-1;
         }
         checksum += fuse_data[index] & mask;
      }
      return checksum;
   }

   /**
    * Print fuse map
    */
   void printFuses(unsigned maxIndex = 0) const;

};

#endif /* BITARRAY_H_ */
