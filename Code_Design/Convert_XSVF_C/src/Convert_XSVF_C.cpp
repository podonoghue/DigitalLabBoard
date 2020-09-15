//============================================================================
// Name        : Convert_XSVF_C.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>

int main(int argc, const char *args[]) {

   //   printf("Argc = %d\n", argc);
   //   printf("Args[0] = %s\n", args[0]);

   const char *filename = "CPLD_Tester.xsvf";
   if (argc>1) {
      filename = args[1];
   }

   FILE *fp = fopen(filename, "rb");

   if (fp ==nullptr) {
      fprintf(stderr, "Failed to open file %s\n", filename);
   }
   fseek(fp, 0L, SEEK_END);
   unsigned long size = ftell(fp);

   FILE *of = fopen("XSVF_Data2.cpp", "wt");
   fprintf(of,
         "/* \n"
         " * XSVF_Data2.cpp \n"
         " *\n"
         " * Derived from %s\n"
         " */ \n"
         "#include <stdint.h>\n"
         "#include \"XSVF_Data2.h\" \n"
         " \n"
         "const uint8_t xsvf_data2[%lu] = {", filename, size);

   rewind(fp);
   unsigned index = 0;
   do {
      int data = fgetc(fp);
      if (data == EOF) {
         break;
      }
      if ((index++%16) == 0) {
         fprintf(of, "\n   /* %6d */ ", index-1);
      }
      fprintf(of, "0x%02X, ", data);
   } while (true);
   fprintf(of, "\n};\n");

   fclose(of);
   fclose(fp);

   of = fopen("XSVF_Data2.h", "wt");
   fprintf(of,
         "/*\n"
         " * XSVF_Data2.h\n"
         " *\n"
         " *  Created on: 16 Dec 2019\n"
         " *      Author: podonoghue\n"
         " */\n"
         "\n"
         "#ifndef SOURCES_XSVF_DATA2_H_\n"
         "#define SOURCES_XSVF_DATA2_H_\n"
         "\n"
         "#include <stdint.h>\n"
         "\n"
         "extern const uint8_t xsvf_data2[%lu];\n"
         "\n"
         "#endif /* SOURCES_XSVF_DATA2_H_ */\n",
         size
   );
   fclose(of);

   return 0;
}
