/** ============================================================================
 * JedecFile.cpp
 *
 *  Created on: 29 Dec 2019
 *      Author: podonoghue
 *  ============================================================================ */
#include <cctype>
#include <cstring>

#include "JedecFile.h"
#include "DeviceMapFile.h"

using namespace std;

/**
 * Read JEDEC file
 *
 * @param jedec_filepath FILE to read
 *
 * @return nullptr    => success
 * @return != nullptr => error message
 */
const char *JedecFile::loadFile(const char *jedec_filepath) {
   FILE *fp = fopen(jedec_filepath, "rb");
   if (fp == nullptr) {
      return "Failed to open file";
   }
   const char *msg = loadFile(fp);
   fclose(fp);

   if (msg != nullptr) {
      return msg;
   }
   if (calculateChecksum() != check_sum) {
      msg = "Invalid checksum";
   }
   return msg;
}

/**
 * Read JEDEC file from fp
 *
 * @param fp FILE to read
 *
 * @return nullptr    => success
 * @return != nullptr => error message
 */
const char *JedecFile::loadFile(FILE *fp) {
   char     comment[1000];
   char     *comment_ptr   = comment;
   unsigned fuse_number    = 0;

   State state       = S_HEADER;
   bool stripSpace   = true;

   auto saveCharacter = [&comment_ptr, &comment](char ch) {
      if (comment_ptr<(comment+sizeof(comment)-1)) {
         *comment_ptr++ = ch;
      }
   };

   auto resetBuffer = [&comment_ptr, &comment]() {
      comment_ptr = comment;
   };

   auto stripTrailing = [&comment_ptr]() {
      do {
         *comment_ptr-- = '\0';
      } while (isspace(*comment_ptr));
   };

   int ch;
   do {
      ch = fgetc(fp);
      if ((ch == ETX) || (ch == -1)) {
         break;
      }
      if ((ch =='\r') || (ch == '\n')) {
         continue;
      }
      switch (state) {
         case S_HEADER:
            if (ch == STX) {
               state = S_COMMAND;
            }
            continue;
            break;
         case S_COMMAND:
         {
            switch(ch) {
               case 'N':
                  state          = S_COMMENT;
                  resetBuffer();
                  saveCharacter('N');
                  stripSpace     = false;
                  break;
               case 'Q':   state = S_QCOMMAND;      break;
               case 'F':   state = S_FUSEDEFAULT;   break;
               case 'X':   state = S_X;             break;
               case 'J':   state = S_J_arch;        break;
               case 'C':   state = S_CHECKSUM;      break;
               case 'L':
                  state = S_L_num;
                  fuse_number = 0;
                  break;
               default:
                  return "Unexpected command character";
            }
            continue;
         }
         case S_CHECKSUM:
            if (ch == '*') {
               state = S_COMMAND;
               continue;
            }
            if ((ch>='0')&&(ch<='9')) {
               check_sum = 16*check_sum + (ch-'0');
               continue;
            }
            if ((ch>='a')&&(ch<='f')) {
               check_sum = 16*check_sum + (ch-'a') + 10;
               continue;
            }
            if ((ch>='A')&&(ch<='F')) {
               check_sum = 16*check_sum + (ch-'A') + 10;
               continue;
            }
            else {
               return "Expected digit";
            }
            break;
         case S_COMMENT:
            if (ch == '*') {
               state = S_COMMAND;
               // Strip trailing spaces
               stripTrailing();
               //printf("Note: \'%s\'\n", comment);
               continue;
            }
            // Strip leading spaces
            if (isspace(ch) && stripSpace) {
               continue;
            }
            stripSpace = false;
            saveCharacter(ch);
            if (strncmp(comment,"N ",2) == 0) {
               // Strip 'Note' from front on special note lines
               resetBuffer();
               stripSpace  = true;
            }
            if (strncmp(comment,"Note ",5) == 0) {
               // Strip 'Note' from front on special note lines
               resetBuffer();
               stripSpace  = true;
            }
            if (strncmp(comment,"DEVICE",6) == 0) {
               resetBuffer();
               stripSpace  = true;
               state = S_COMMENT_device;
            }
            if (strncmp(comment,"VERSION",7) == 0) {
               resetBuffer();
               stripSpace  = true;
               state = S_COMMENT_version;
            }
            continue;
         case S_COMMENT_version:
            if (ch == '*') {
               state = S_COMMAND;
               saveCharacter('\0');
               version_info = strdup(comment);
               //printf("Version: \'%s\'\n", comment);
               continue;
            }
            if (isspace(ch) && stripSpace) {
               continue;
            }
            stripSpace = false;
            saveCharacter(ch);
            continue;
         case S_COMMENT_device:
            if (ch == '*') {
               state = S_COMMAND;
               saveCharacter('\0');
               device_name = strdup(comment);
               //printf("Device: \'%s\'\n", comment);
               continue;
            }
            if (isspace(ch) && stripSpace) {
               continue;
            }
            stripSpace = false;
            saveCharacter(ch);
            continue;
         case S_QCOMMAND:
         {
            switch(ch) {
               case 'F': state = S_QF;    break;
               case 'P': state = S_QP;    break;
               case 'V': state = S_QV;    break;
               default:  return "Unexpected character in Q..";
            }
            continue;
         }
         case S_QF:
            if (isdigit(ch)) {
               fuse_limit = 10*fuse_limit+(ch-'0');
               continue;
            }
            if (ch == '*') {
               fuse_data.allocateFuses(fuse_limit);
               //printf("fuse_limit = %d\n", fuse_limit);
               state = S_COMMAND;
               continue;
            }
            return "Expected digit";
         case S_QP:
            if (ch == '*') {
               state = S_COMMAND;
               //printf("number_of_pins = %d\n", number_of_pins);
               continue;
            }
            number_of_pins = 10*number_of_pins+(ch-'0');
            continue;
         case S_QV:
            if (isdigit(ch)) {
               vector_limit = 10*vector_limit+(ch-'0');
               continue;
            }
            if (ch == '*') {
               state = S_COMMAND;
               //printf("vector_limit = %d\n", vector_limit);
               continue;
            }
            return "Expected digit";
         case S_X:
            if (ch == '*') {
               state = S_COMMAND;
               //printf("default_test = %d\n", default_test);
               continue;
            }
            default_test = (ch - '0');
            continue;
         case S_J_arch:
            if (isdigit(ch)) {
               device_code_arch = 10*device_code_arch + (ch-'0');
               continue;
            }
            if (ch == ' ') {
               state = S_J_pin;
               continue;
            }
            return "Expected digit";
         case S_J_pin:
            if (isdigit(ch)) {
               device_code_pin = 10*device_code_pin + (ch-'0');
               continue;
            }
            if (ch == '*') {
               state = S_COMMAND;
               //printf("device_identification = %d.%d\n", device_code_arch, device_code_pin);
               continue;
            }
            return "Expected digit";
         case S_L_num:
            if (isdigit(ch)) {
               fuse_number = 10*fuse_number + (ch-'0');
               continue;
            }
            if (isspace(ch)) {
               state = S_L_value;
               //printf("fuse_number = %d\n", fuse_number);
               continue;
            }
            return "Expected digit";
         case S_L_value:
            if (ch == '*') {
               state = S_COMMAND;
               continue;
            }
            if (isspace(ch)) {
               continue;
            }
            if (fuse_number>(fuse_limit)) {
               return "Fuse number exceeds expected number of fuses";
            }
            if ((ch == '0')||(ch == '1')) {
               fuse_data.setFuse(fuse_number++, (ch == '1'));
               continue;
            }
            return "Expected 0/1 digit";
         case S_FUSEDEFAULT:
            if ((ch == '0')||(ch == '1')) {
               default_state = (ch - '0');
               continue;
            }
            if (ch == '*') {
               state = S_COMMAND;
               fuse_data.clearFuses(default_state);
               //printf("default_state = %d\n", default_state);
               continue;
            }
            return "Expected 0/1 digit";
      }
      putchar(ch);
   } while (true);
   //printf("\nETX ================\n");

   return nullptr;
}

/**
 * Report file contents
 */
void JedecFile::report() {
   printf("Fuse limit      - %d\n",     fuse_limit);
   printf("Number of pins  - %d\n",     number_of_pins);
   printf("Device code     - %d.%d\n",  device_code_arch, device_code_pin);
   printf("Device name     - %s\n",     device_name);
   printf("Checksum        - 0x%04X\n", check_sum);
   fuse_data.printFuses();
}

/**
 * Report Programming data
 */
void JedecFile::printProgrammingData(unsigned max) {
   printf("Programming Data:\n");
   programming_data.printFuses(max);
}

/**
 * Print fuse map
 */
void JedecFile::jedecPrintFuses() {
   static constexpr unsigned WIDTH = 100;

   auto doHeader = []() {
      printf("N       ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum+=10) {
         printf("%-10X", fuseNum/10);
      }
      printf("\nN       ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum++) {
         printf("%1X", fuseNum%10);
      }
      printf("\nN       ");
      for (unsigned fuseNum=0; fuseNum<WIDTH; fuseNum++) {
         printf("-");
      }
      printf("\n");
   };

   bool needIntro = true;
   unsigned rows = 0;

   for (unsigned fuseNum=0; fuseNum<fuse_limit; fuseNum++) {

      if(needIntro) {
         if ((rows++%100) == 0) {
            doHeader();
         }
         printf("L%06d ", fuseNum);
      }
      printf("%d",getFuse(fuseNum)?1:0);
      needIntro = (fuseNum%WIDTH) == (WIDTH-1);
      if (needIntro) {
         printf("*\n");
      }
   }
   printf("*\n");
   printf("C%04X*\n", check_sum);
}

/**
 * Print JEDEC file
 */
void JedecFile::jedecPrint() {
   printf("Programmer Jedec Bit Map\n");
   printf("%c", STX);
   printf("QF%d*\n", fuse_limit);
   printf("QP%d*\n", number_of_pins);
   printf("QV%d*\n", vector_limit);
   printf("F%d*\n", default_state);
   printf("X%d*\n", default_test);
   printf("J%d %d*\n", device_code_arch, device_code_pin);
   printf("N VERSION %s*\n", version_info);
   printf("N DEVICE %s*\n\n", device_name);
   jedecPrintFuses();
   printf("%c", ETX);
}

/**
 * Create programming data from fuse data using mapping file
 *
 * @param mapFile  File used to map JEDEC fuses to programming bit map
 */
void JedecFile::rearrangeProgrammingData(DeviceMapFile &mapFile) {
   mapFile.jedecfile2bitfile(*this, programming_data);
}
