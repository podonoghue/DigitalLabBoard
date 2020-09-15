/*
 * Xsvf.h
 *
 *  Created on: 9 Jan 2020
 *      Author: podonoghue
 */

#ifndef XSVF_H_
#define XSVF_H_
#include <cstdint>
#include <cassert>
#include <cstring>
#include <memory>
#include <cstdio>

class Xsvf {

private:
   std::uint8_t *xsvf_data;
   unsigned      xsvf_max_size;
   unsigned      xsvf_size;

public:
   /**
    * Construct empty XSVF
    *
    * @param defaultSize  Default size when buffer is initially allocated
    */
   Xsvf(unsigned defaultSize = 10000) : xsvf_data(nullptr), xsvf_max_size(defaultSize), xsvf_size(0) {
      assert(defaultSize > 100);
   }

   Xsvf &operator=(const Xsvf& other) {
      xsvf_data      = nullptr;
      xsvf_max_size  = other.xsvf_max_size;
      xsvf_size      = other.xsvf_size;
      if (xsvf_max_size > 0) {
         xsvf_data = (uint8_t *)malloc(xsvf_max_size);
         std::memcpy(xsvf_data, other.xsvf_data, xsvf_size);
      }
      return *this;
   }

   Xsvf(const Xsvf& other) {
      this->operator =(other);
   }

   /**
    * Destructor
    */
   virtual ~Xsvf() {
      if (xsvf_data != nullptr) {
         std::free(xsvf_data);
      }
   }

   /**
    * Clear (empty) buffer
    */
   void clear() {
      xsvf_size = 0;
   }

   /**
    * Append uint8_t to XSVF file
    *
    * @param value Value to append
    */
   void writeByte(uint8_t value) {
      if (xsvf_data == nullptr) {
         xsvf_data = (uint8_t *)malloc(xsvf_max_size);
         xsvf_size = 0;
      }
      if (xsvf_size >= xsvf_max_size) {
         xsvf_data = (uint8_t *) std::realloc(xsvf_data, 2*xsvf_max_size);
         xsvf_max_size = 2*xsvf_max_size;
      }
      assert(xsvf_data != nullptr);
      xsvf_data[xsvf_size++] = value;
   }

   /**
    * Append uint32_t to XSVF file
    *
    * @param value Value to append
    */
   void writeLong(uint32_t value) {
      writeByte((uint32_t)(value>>24));
      writeByte((uint32_t)(value>>16));
      writeByte((uint32_t)(value>>8));
      writeByte((uint32_t)(value));
   }

   /**
    * Append bytes to XSVF file
    *
    * @param size    Number of bytes
    * @param value   Data to append
    */
   void writeBytes(unsigned size, const uint8_t *value) {
      while (size-->0) {
         writeByte(*value++);
      }
   }

   std::uint8_t* getData() const {
      return xsvf_data;
   }

   unsigned getSize() const {
      return xsvf_size;
   }

   bool isEmpty() {
      return xsvf_size == 0;
   }

   const char *writeFile(const char *filePath) {
      const char *msg = nullptr;
      FILE *fp = nullptr;
      do {
         fp = std::fopen(filePath, "wb");
         if (fp == nullptr) {
            msg = "Failed to open XSVF file";
            continue;
         }
         std::fwrite(getData(), 1, getSize(), fp);
         if (ferror(fp)) {
            msg = "Error reading file";
            continue;
         }
      } while(false);
      if (fp != nullptr) {
         fclose(fp);
      }
      return msg;
   }

   const char *readFile(const char *filePath) {
      const char *msg = nullptr;

      clear();

      FILE *fp = nullptr;
      do {
         fp = std::fopen(filePath, "rb");
         if (fp == nullptr) {
            msg = "File failed to open";
            continue;
         }
         do {
            uint8_t buff[1000];
            size_t bytesRead = std::fread(buff, 1, sizeof(buff), fp);
            if (bytesRead == 0) {
               break;
            }
            writeBytes(bytesRead, buff);
         } while (true);
         if (ferror(fp)) {
            msg = "Error reading file";
            clear();
         }
      } while (false);
      if (fp != nullptr) {
         fclose(fp);
      }
      return msg;
   }
};

#endif /* XSVF_H_ */
