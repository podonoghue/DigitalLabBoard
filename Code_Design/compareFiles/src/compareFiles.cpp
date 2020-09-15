//============================================================================
// Name        : compareFiles.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>

#include "XSVF_Data.h"
#include "XSVF_Data2.h"

int main() {
   for (unsigned index=0; index<sizeof(xsvf_data); index++) {
      if (xsvf_data[index] != xsvf_data2[index]) {
         printf("Failed @%d\n", index);
      }
   }
}
