/*
 * XsvfWriter.cpp
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */

#include <malloc.h>
#include <stdio.h>
#include "JtagTables.h"
#include "XsvfWriter.h"

void XsvfWriter::clear() {
   xsvf.clear();
}

#define BYTES32(x) ((uint8_t)((x)>>24)),((uint8_t)((x)>>16)),((uint8_t)((x)>>8)),((uint8_t)(x))

void XsvfWriter::doPreamble() {
   static const uint8_t preamble[] = {
         /* Reset           */ XRUNTEST, BYTES32(0), // cycle/ms
         /* Reset           */ XREPEAT, 0,
         /* Reset           */ XENDIR, Ex_Idle,
         /* Reset           */ XENDDR, Ex_Idle,
         /* Reset           */ XSTATE, Reset, // *** Reset ***
         /* Reset           */ XSTATE, Idle,
         /* Idle            */
   };
   xsvf.writeBytes(sizeof(preamble), preamble);
}

void XsvfWriter::doPostamble() {
   static const uint8_t postamble[] = {
         /* Idle            */ XSTATE, Idle,
         /* Reset           */ XSTATE, Reset,   // Reset JTAG
         /* Idle            */ XCOMPLETE,
   };
   xsvf.writeBytes(sizeof(postamble), postamble);
}

void XsvfWriter::doErase() {
   static const uint8_t erase[] = {
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ ENABLE_COMMAND, // 0xE8
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Pause
         /* tdi_value     8:*/ ERASE_COMMAND, // 0xED
         /* IR_Pause        */ XENDIR, Ex_Idle,
         /* IR_Pause        */ XWAIT, DR_Pause, DR_Pause, BYTES32(20), // -IR_Exit2-IR_Update-DR_Select-DR_Capture-DR_Exit1-DR_Pause-Wait(20)
         /* DR_Pause        */ XWAIT, Idle, Idle, BYTES32(100000), // -DR_Exit2-DR_Update-Idle-Wait(100000)
         /* Idle            */ XWAIT, DR_Pause, DR_Pause, BYTES32(5000), // -DR_Select-DR_Capture-DR_Exit1-DR_Pause-Wait(5000)
         /* DR_Pause        */ XWAIT, Idle, Idle, BYTES32(1), // -DR_Exit2-DR_Update-Idle-Wait(1)
         /* Idle            */
   };
   xsvf.writeBytes(sizeof(erase), erase);
}

void XsvfWriter::confirmIdcode(const DeviceInformation &deviceInformation) {
   const uint8_t erase[] = {
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ IDCODE_COMMAND, // 0x01
         /* Idle            */ XSDRSIZE, BYTES32(32),
         /* Idle            */ XTDOMASK,
         /* tdo_mask     32:*/ BYTES32(deviceInformation.getIdcodeMask()),
         /* Idle            */ XSDRTDO, // -DR_Select-DR_Capture-DR_Shift-Shift(32)-DR_Update-Idle
         /* tdi_value    32:*/ BYTES32(0x00000000),
         /* tdo_value    32:*/ BYTES32(deviceInformation.getIdcode()),
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ BYPASS_COMMAND, // 0xFF
         /* Idle            */
   };
   xsvf.writeBytes(sizeof(erase), erase);
}

void XsvfWriter::writeFill(unsigned postSize, unsigned padding, unsigned blockSize) {
   bool flushByte = false;
   unsigned index;
   uint8_t byte  = 0;
   for (index=(padding+1); index<=(postSize+padding); index++) {
      if (flushByte) {
         xsvf.writeByte(byte);
      }
      flushByte = (index%8) == 0;
   }
   for (int bitNum=blockSize-1; bitNum>=0; bitNum--) {
      if (flushByte) {
         xsvf.writeByte(byte);
      }
      flushByte = (index%8) == 0;
      index++;
   }
   if (flushByte) {
      xsvf.writeByte(byte);
   }

}
void XsvfWriter::program(JedecFile &jedecFile, const DeviceInformation &deviceInformation) {
   static const uint8_t preamble[] = {
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Pause
         /* tdi_value     8:*/ INIT_COMMAND,    // 0xF0
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(20), // -Wait(20)
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Pause
         /* tdi_value     8:*/ INIT_COMMAND,    // 0xF0
         /* IR_Pause        */ XSTATE, IR_Exit2,
         /* IR_Exit2        */ XSTATE, IR_Update,
         /* IR_Update       */ XSTATE, DR_Select,
         /* DR_Select       */ XSTATE, DR_Capture,
         /* DR_Capture      */ XSTATE, DR_Exit1,
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(800), // -Wait(800)
         /* Idle            */ XENDIR, Ex_Idle,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ CONLD_COMMAND,   // 0xC0
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(100), // -Wait(100)
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ CONLD_COMMAND,   // 0xC0
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(100), // -Wait(100)
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Update-Idle
         /* tdi_value     8:*/ ENABLE_COMMAND,  // 0xE8
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-IR_Pause
         /* tdi_value     8:*/ PROGRAM_COMMAND, // 0xEA
   };
   static const uint8_t postamble[] = {

         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Pause
         /* tdi_value     8:*/ INIT_COMMAND,    // 0xF0
         /* IR_Pause        */ XSTATE, IR_Exit2,
         /* IR_Exit2        */ XSTATE, IR_Update,
         /* IR_Update       */ XSTATE, Idle,
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(20), // -Wait(20)
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Pause
         /* tdi_value     8:*/ INIT_COMMAND,    // 0xF0
         /* IR_Pause        */ XSTATE, IR_Exit2,
         /* IR_Exit2        */ XSTATE, IR_Update,
         /* IR_Update       */ XSTATE, DR_Select,
         /* DR_Select       */ XSTATE, DR_Capture,
         /* DR_Capture      */ XSTATE, DR_Exit1,
         /* DR_Exit1        */ XSTATE, DR_Update,
         /* DR_Update       */ XSTATE, Idle,
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(800), // -Wait(800)
         /* Idle            */ XENDIR, Ex_Idle,

         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ CONLD_COMMAND,   // 0xC0
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(100), // -Wait(100)
         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ CONLD_COMMAND,   // 0xC0
         /* Idle            */ XWAIT, Idle, Idle, BYTES32(100), // -Wait(100)

         /* Idle            */ XSIR, 8,         // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ BYPASS_COMMAND,  // 0xC0
   };

   GrayCounter counter;

   const unsigned postSize  = deviceInformation.getPost();
   const unsigned blockSize = deviceInformation.getBlockSize();

   xsvf.writeBytes(sizeof(preamble), preamble);
   xsvf.writeByte(XSDRSIZE);   xsvf.writeLong(blockSize+postSize);
   xsvf.writeByte(XENDIR);     xsvf.writeByte(Ex_Idle);

   int fusesToDo = jedecFile.getProgramSize();

   // TDO mask
   xsvf.writeByte(XTDOMASK);

   const unsigned padding = (8-((postSize+blockSize)%8))%8;

   writeFill(postSize, padding, blockSize);

   unsigned fuseIndex = 0;

   unsigned row = 0;
   while(fusesToDo>0) {
      bool flushByte = false;
      unsigned index;
      uint8_t  byte  = 0;
      unsigned postValue = counter.getValue();

      flushByte = false;
      index = (padding+1);
      byte  = 0;

      xsvf.writeByte(XSDRTDO);

      byte  = 0;
      flushByte = false;
      for (index=(padding+1); index<=(postSize+padding); index++) {
         if (flushByte) {
            xsvf.writeByte(byte);
//            printf(",");
   //         printf("0x%02X,", byte);
            byte = 0;
         }
         byte = (byte<<1) | (postValue&0b1);
//         printf("%d", (postValue&0b1));

         postValue >>= 1;
         flushByte = (index%8) == 0;
      }
//      printf("/");
      for (int bitNum=blockSize-1; bitNum>=0; bitNum--) {
         if (flushByte) {
            xsvf.writeByte(byte);
//            printf(",");
   //         printf("0x%02X,", byte);
            byte = 0;
         }
         unsigned bit;
         if ((fuseIndex+bitNum)>jedecFile.getProgramSize()) {
            bit = 0b1;
         }
         else {
            bit = jedecFile.getProgramBit(fuseIndex+bitNum);
         }
         byte = (byte<<1) | bit;
   //      byte = (byte>>1) | (jedecFile.getProgramBit(bitNum)?0b10000000U:0b0U);
//         printf("%1d", bit);
         flushByte = (index%8) == 0;
         index++;
      }
      if (flushByte) {
         xsvf.writeByte(byte);
      }

      writeFill(postSize, padding, blockSize);

      xsvf.writeByte(XWAIT); xsvf.writeByte(Idle); xsvf.writeByte(Idle); xsvf.writeLong(10000);
      fusesToDo -= blockSize;
      fuseIndex += blockSize;
//      printf(" row = %6d\n", row);
      row++;
      //      printf("\n");
      counter.increment();

   }
   xsvf.writeBytes(sizeof(postamble), postamble);
}

void XsvfWriter::verify(JedecFile &jedecFile, const DeviceInformation &deviceInformation) {
   static const uint8_t preamble[] = {
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ ENABLE_COMMAND, // 0xE8
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ ENABLE_COMMAND, // 0xE8
         /* Idle            */ XENDIR, Ex_IR_Pause,
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Pause
         /* tdi_value     8:*/ VERIFY_COMMAND, // 0xEE
         /* DR_Pause        */ XENDIR, Ex_Idle,
   };

   GrayCounter counter;

   const unsigned blockSize = deviceInformation.getBlockSize();

   xsvf.writeBytes(sizeof(preamble), preamble);

   int fusesToDo = jedecFile.getProgramSize();

   unsigned fuseIndex = 0;
   unsigned row = 0;
   while(fusesToDo>0) {
//      printf(" row = %6d: ", row);
      const uint8_t verifyPreamble[] = {
            /* IR_Pause        */ XENDDR, Ex_DR_Pause,
            /* IR_Pause        */ XSDRSIZE, BYTES32(deviceInformation.getPost()),
            /* IR_Pause        */ XTDOMASK,
            /* tdo_mask   post:*/ 0x00,
            /* IR_Pause        */ XSDRTDO, // -IR_Exit2-IR_Update-DR_Select-DR_Capture-DR_Shift-Shift(7)-DR_Exit1-DR_Pause
            /* tdi_value  post:*/ (uint8_t)counter.getReversedValue(deviceInformation.getPost()),
            /* tdo_value  post:*/ 0x00,
            /* DR_Pause        */ XWAIT, DR_Pause, DR_Pause, BYTES32(20), // -Wait(20)
            /* DR_Pause        */ XENDDR, Ex_Idle,
            /* DR_Pause        */ XWAIT, Idle, Idle, BYTES32(100), // -DR_Exit2-DR_Update-Idle-Wait(100)
            /* Idle            */ XSDRSIZE, BYTES32(blockSize),
      };
      xsvf.writeBytes(sizeof(verifyPreamble), verifyPreamble);

      const unsigned padding = (8-(blockSize%8))%8;
      uint8_t  byte;
      unsigned index;
      bool flushByte;

      // TDO mask
      xsvf.writeByte(XTDOMASK);
      flushByte = false;
      index = (padding+1);
      byte  = 0;
      for (int bitNum=blockSize-1; bitNum>=0; bitNum--) {
         if (flushByte) {
            xsvf.writeByte(byte);
//            printf(",");
            //         printf("0x%02X,", byte);
            byte = 0;
         }
         byte = (byte<<1) | ((row<deviceInformation.getBlockNum())?0b1:0b0);
//         printf("%1d", 0b1);
         flushByte = (index%8) == 0;
         index++;
      }
      if (flushByte) {
         xsvf.writeByte(byte);
      }

      // TDI dummy data
      xsvf.writeByte(XSDRTDO);
      flushByte = false;
      index=(padding+1);
      byte  = 0;
      for (int bitNum=blockSize-1; bitNum>=0; bitNum--) {
         if (flushByte) {
            xsvf.writeByte(byte);
//            printf(",");
//         printf("0x%02X,", byte);
            byte = 0;
         }
         byte = (byte<<1) | 0b1;
//         printf("%1d", 0b1);
         flushByte = (index%8) == 0;
         index++;
      }
      if (flushByte) {
         xsvf.writeByte(byte);
      }

      // TDO expected data
      flushByte = false;
      index=(padding+1);
      byte  = 0;
      for (int bitNum=blockSize-1; bitNum>=0; bitNum--) {
         if (flushByte) {
            xsvf.writeByte(byte);
//            printf(",");
            //         printf("0x%02X,", byte);
            byte = 0;
         }
         unsigned bit;
         if (row<deviceInformation.getBlockNum()) {
            bit = jedecFile.getProgramBit(fuseIndex+bitNum);
         }
         else {
            // Dummy the done and user data rows
            bit = 0b0;
         }
//         if ((fuseIndex+bitNum)>jedecFile.getProgramSize()) {
//            bit = 0b1;
//         }
//         else {
//            bit = jedecFile.getProgramBit(fuseIndex+bitNum);
//         }
         byte = (byte<<1) | bit;
//         printf("%1d", 0b1);
         flushByte = (index%8) == 0;
         index++;
      }
      if (flushByte) {
         xsvf.writeByte(byte);
      }
      xsvf.writeByte(XWAIT); xsvf.writeByte(Idle); xsvf.writeByte(Idle); xsvf.writeLong(100);
      fusesToDo -= blockSize;
      fuseIndex += blockSize;
//      printf("\n");
      counter.increment();
      row++;
   }
   const uint8_t postamble[] = {
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ CONLD_COMMAND, // 0xC0
         /* Idle            */ XSIR, 8, // -DR_Select-IR_Select-IR_Capture-IR_Shift-Shift(8)-DR_Exit1-IR_Update-Idle
         /* tdi_value     8:*/ BYPASS_COMMAND, // 0xFF
   };
   xsvf.writeBytes(sizeof(postamble), postamble);
//   printf("\n");
}
