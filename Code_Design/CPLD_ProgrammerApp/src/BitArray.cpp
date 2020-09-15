/*
 * BitArray.cpp
 *
 *  Created on: 30 Dec 2019
 *      Author: podonoghue
 */
#include <cstdio>
#include "BitArray.h"

using namespace std;

/**
 * Print fuse map
 */
void BitArray::printFuses(unsigned maxIndex) const {
   static constexpr unsigned WIDTH = 64;

   static auto doHeader = []() {
      printf("      ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum++) {
         if ((fuseNum%10) == 0) {
            printf("%1d", fuseNum/10);
         }
         else {
            printf(" ");
         }
         if ((fuseNum%8) == 7) {
            printf(" ");
         }
      }
      printf("\n      ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum++) {
         printf("%1d", fuseNum%10);
         if ((fuseNum%8) == 7) {
            printf(" ");
         }
      }
      printf("\n      ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum++) {
         printf("-");
         if ((fuseNum%8) == 0) {
            printf(" ");
         }
      }
      printf("\n");
   };

   bool needIntro = true;
   unsigned row = 0;

   if (maxIndex == 0) {
      maxIndex = fuse_limit;
   }
   printf("Fuses: \n");
   for (unsigned fuseNum=0; fuseNum<maxIndex; fuseNum++) {
      if(needIntro) {
         if ((row++%20) == 0) {
            doHeader();
         }
         printf("%05d:", fuseNum);
      }
      printf("%d",getFuse(fuseNum)?1:0);
      if ((fuseNum%8) == 7) {
         printf(" ");
      }
      needIntro = (fuseNum%WIDTH) == (WIDTH-1);
      if (needIntro) {
         printf("\n");
      }
   }
   if (!needIntro) {
      printf("\n");
   }
}




