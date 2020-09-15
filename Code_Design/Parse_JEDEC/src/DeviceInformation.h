/*
 * DeviceInformation.h
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */

#ifndef DEVICEINFORMATION_H_
#define DEVICEINFORMATION_H_

#include <cstdint>

class DeviceInformation {

private:
   const char    *name;          // Name of device
   unsigned       name_length;   // Length of name
   unsigned       block_size;    // Size of a block (rows in map file)
   unsigned       block_num;     // Number of blocks (columns in map file excluding special fuses)
   unsigned       post;          // Number of extra bits during programming
   std::uint32_t  idcode;        // IDCODE for device
   std::uint32_t  idcodeMask;    // Mask of valid bits on IDCODE
   const char    *mapfilename;   // Name of device map file

   /**
    * Constructor
    *
    * @param name             Name of device
    * @param name_length      Length of name
    * @param block_size       Size of a block (rows in map file)
    * @param block_num        Number of blocks (columns in map file excluding special fuses)
    * @param post             Number of extra bits during programming
    * @param idcode           IDCODE for device
    * @param idcodeMask       Mask of valid bits on IDCODE
    * @param mapfilename      Name of Xilinx device map file to permutate bits for programming
    */
   constexpr DeviceInformation(
         const char    *name,
         unsigned       name_length,
         unsigned       block_size,
         unsigned       block_num,
         unsigned       post,
         std::uint32_t  idcode,
         std::uint32_t  idcodeMask,
         const char    *mapfilename) :
            name(name),
            name_length(name_length),
            block_size(block_size),
            block_num(block_num),
            post(post),
            idcode(idcode),
            idcodeMask(idcodeMask),
            mapfilename(mapfilename) {
   }

   static const DeviceInformation deviceInformation[];

public:
   /**
    * Find information for device
    *
    * @param deviceName  Name of device
    *
    * @return  Information
    */
   static const DeviceInformation *findDevice(const char *deviceName);

   /**
    * Find information for device
    *
    * @param idcode  IDCODE of device
    *
    * @return  Information
    */
   static const DeviceInformation *findDevice(std::uint32_t idcode);

   unsigned getBlockNum() const {
      return block_num;
   }

   unsigned getBlockSize() const {
      return block_size;
   }

   const char* getMapfilename() const {
      return mapfilename;
   }

   const char* getName() const {
      return name;
   }

   unsigned getNameLength() const {
      return name_length;
   }

   unsigned getPost() const {
      return post;
   }

   std::uint32_t getIdcode() const {
      return idcode;
   }

   std::uint32_t getIdcodeMask() const {
      return idcodeMask;
   }
};

#endif /* DEVICEINFORMATION_H_ */
