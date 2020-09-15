/*
 ============================================================================
 * @file    xsvf_Parse.cpp
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include "XsvfParser.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <string>

/**
 * Get command name as string
 *
 * @param command Command code
 *
 * @return Pointer to static string
 */
const char *XsvfParser::getXCommandName(XsvfCommand command) {
   static const char* commands[] = {
         "XCOMPLETE",     // 0x00
         "XTDOMASK",      // 0x01
         "XSIR",          // 0x02
         "XSDR",          // 0x03
         "XRUNTEST",      // 0x04
         "Illegal",
         "Illegal",
         "XREPEAT",       // 0x07
         "XSDRSIZE",      // 0x08
         "XSDRTDO",       // 0x09
         "XSETSDRMASKS",  // 0x0a
         "XSDRINC",       // 0x0b
         "XSDRB",         // 0x0c
         "XSDRC",         // 0x0d
         "XSDRE",         // 0x0e
         "XSDRTDOB",      // 0x0f
         "XSDRTDOC",      // 0x10
         "XSDRTDOE",      // 0x11
         "XSTATE",        // 0x12
         "XENDIR",        // 0x13
         "XENDDR",        // 0x14
         "XSIR2",         // 0x15
         "XCOMMENT",      // 0x16
         "XWAIT",         // 0x17
   };

   const char *cmdName = "Illegal";
   if (((unsigned)command)<(sizeof(commands)/sizeof(commands[0]))) {
      cmdName = commands[command];
   }
   return (cmdName);
}

/**
 * Get state name as string
 *
 * @param state State code
 *
 * @return Pointer to static string
 */
const char *XsvfParser::getXstateName(Xstate state) {
   static const char* names[] = {
         "Reset",
         "Idle",
         "DR_Select",
         "DR_Capture",
         "DR_Shift",
         "DR_Exit1",
         "DR_Pause",
         "DR_Exit2",
         "DR_Update",
         "IR_Select",
         "IR_Capture",
         "IR_Shift",
         "IR_Exit1",
         "IR_Pause",
         "IR_Exit2",
         "IR_Update",
   };
   const char *name = "Illegal";
   if (((unsigned)state)<(sizeof(names)/sizeof(names[0]))) {
      name = names[state];
   }
   return (name);
}

/**
 * Print buffer contents in binary
 *
 * @param title Title to prefix each line
 * @param size	Size of buffer
 * @param buff	Buffer to print
 */
void XsvfParser::printBits(const char* title, unsigned numBits, const uint8_t *buff) {
   unsigned size = (numBits+7)/8;
   unsigned index = 8*size;
   bool doHeader = true;
   unsigned byteCount = 0;
   while(size-->0) {
      if (doHeader) {
         fprintf(fp_output, "   /* %-10s %4d:*/ ", title, (index>numBits)?numBits:index);
      }
      index -= 8;
      fprintf(fp_output, "0x%02X, ", *buff++);
      doHeader = (++byteCount == 8);
      if (doHeader) {
         byteCount = 0;
         fprintf(fp_output, "\n");
      }
   }
   fprintf(fp_output, "\n");
}

/**
 * Print buffer contents in binary
 *
 * @param title Title to prefix each line
 * @param size Size of buffer
 * @param buff Buffer to print
 */
void XsvfParser::printBitsAsComment(const char* title, unsigned numBits, const uint8_t *buff) {
   unsigned size = (numBits+7)/8;
   unsigned index = 0;
   bool doHeader = true;
   unsigned byteCount = 0;
   const uint8_t *buffPtr = buff+size;
   while (buffPtr>=buff) {
      if (doHeader) {
         fprintf(fp_output, "   // %-10s %4d: ", title, (index>numBits)?numBits:index);
      }
      uint8_t value = *--buffPtr;
      for (unsigned sub=0; sub<8; sub++) {
         if (index++ >= numBits) {
            break;
         }
         fprintf(fp_output, "%1d", value&0b1);
         value >>= 1;
      }
      fprintf(fp_output, " ");
      doHeader = (++byteCount == 8);
      if (doHeader) {
         byteCount = 0;
         fprintf(fp_output, "\n");
      }
   }
   fprintf(fp_output, "\n");
}

/**
 * Shift out value
 * Assumes in DR_Shift or IR_Shift state
 *
 * @param size             Number of bits to shift
 * @param tdi_value        Dummy TDI value to shift in
 * @param tdo_value        Buffer for TDO value shifted out
 * @param exit_shift_state Whether to exit IR/DR_Shift at end of shift
 */
void XsvfParser::shiftOut(unsigned size, bool tdi_value, uint8_t *tdo_value, bool exit_shift_state) {

   assert((currentState == IR_Shift) || (currentState == DR_Shift));
   fprintf(fp_output, "-Shift(%d)", size);

   unsigned sizeInBytes = (size+7)/8;

   // Start at Least Significant bit
   uint8_t mask = 0b1;

   // Start at Least Significant byte
   tdo_value  += sizeInBytes-1;

   while(size-->0) {
      if (mask == 0b1) {
         *tdo_value = 0;
      }
      if (size == 0) {
         // Last bit - move to
      }
      mask <<= 1;
      if (mask == 0) {
         mask = 0b1;
         tdo_value--;
      }
   }
   if (exit_shift_state) {
      currentState = (currentState==IR_Shift)?IR_Exit1:DR_Exit1;
      fprintf(fp_output, (currentState==IR_Shift)?"-IR_Exit1":"-DR_Exit1");
   }
}

/**
 * Shift in value
 * Assumes in DR_Shift or IR_Shift state
 *
 * @param size             Number of bits to shift
 * @param tdi_value        TDI value to shift in
 * @param exit_shift_state Whether to exit IR/DR_Shift at end of shift
 */
void XsvfParser::shiftIn(unsigned size, uint8_t *tdi_value, bool exit_shift_state) {
   assert((currentState == IR_Shift) || (currentState == DR_Shift));
   fprintf(fp_output, "-Shift(%d)", size);
   if (exit_shift_state) {
      currentState = (currentState==IR_Shift)?IR_Exit1:DR_Exit1;
      fprintf(fp_output, (currentState==IR_Shift)?"-IR_Exit1":"-DR_Exit1");
   }
}

/**
 * Shift in value
 * Assumes in DR_Shift or IR_Shift state
 *
 * @param size             Number of bits to shift
 * @param tdi_value        TDI value to shift in
 * @param tdo_value        TDO value to check
 * @param tdo_mask         TDO mask value applied to check
 * @param exit_shift_state Whether to exit IR/DR_Shift at end of shift
 *
 * @return true  => TDO value matched
 * @return false => TDO value did not match
 */
bool XsvfParser::shiftIn(unsigned size, uint8_t *tdi_value, uint8_t *tdo_value, uint8_t *tdo_mask, bool exit_shift_state) {

   assert((currentState == IR_Shift) || (currentState == DR_Shift));
   fprintf(fp_output, "-Shift(%d)", size);
   if (exit_shift_state) {
      currentState = (currentState==IR_Shift)?IR_Exit1:DR_Exit1;
      fprintf(fp_output, (currentState==IR_Shift)?"-IR_Exit1":"-DR_Exit1");
   }
   return true;
}

/**
 * Shift in value
 * Assumes in DR_Shift or IR_Shift state
 *
 * @param size             Number of bits to shift
 * @param tdi_value        TDI value to shift in
 * @param tdo_value        TDO value to check
 * @param exit_shift_state Whether to exit IR/DR_Shift at end of shift
 *
 * @return true  => TDO value matched
 * @return false => TDO value did not match
 */
bool XsvfParser::shiftIn(unsigned size, uint8_t *tdi_value, uint8_t *tdo_value, bool exit_shift_state) {

   assert((currentState == IR_Shift) || (currentState == DR_Shift));
   fprintf(fp_output, "-Shift(%d)", size);
   if (exit_shift_state) {
      currentState = (currentState==IR_Shift)?IR_Exit1:DR_Exit1;
      fprintf(fp_output, (currentState==IR_Shift)?"-IR_Exit1":"-DR_Exit1");
   }
   return true;
}

/**
 * Determine the next state based upon current state and TMS value
 * This does not affect the hardware state
 *
 * @param tms  TMS value controlling state transition
 *
 * @return Calculated state
 */
Xstate XsvfParser::calculateNextState(bool tms) {
   Xstate nextState = stateTable[currentState][tms];
   //      console.WRITE("->{").WRITE(getXstateName(nextState)).WRITE(",").WRITE(tms?'1':'0').WRITELN("}");
   return nextState;
}

/**
 * Move to new state.
 * This outputs signals on TMS, TDO, TCLK to change physical state
 *
 * @param to            State to move to
 * @param isXSTATE      Special formatting for XSTATE
 */
void XsvfParser::moveTo(Xstate to, bool isXSTATE) {
   if (to == Reset) {
      currentState = Reset;
      fprintf(fp_output, " // *** Reset ***");
      return;
   }
   if (currentState == to) {
      // Do nothing
      return;
   }
   // Follow sequence
   do {
      int8_t tms = tapTransitions[currentState][to];
      if (tms<0) {
         fprintf(fp_output, "\n// %s -> %s - illegal transition", getXstateName(currentState), getXstateName(to));
         assert(tms>=0);
      }
      currentState = calculateNextState(tms);
      if (isXSTATE) {
         // Suppress reporting single transitions
         isXSTATE = false;
         if (currentState != to) {
            fprintf(fp_output, " // -%s", getXstateName(currentState));
         }
      }
      else {
         fprintf(fp_output, "-%s", getXstateName(currentState));
      }
   } while(currentState != to);
}

/**
 * Print path leading from 'from' to 'to'
 *
 * @param from Starting state
 * @param to   Ending state
 */
void XsvfParser::printTransition(Xstate from, Xstate to) {
   fprintf(fp_output, getXstateName(from));
   Xstate currentState = from;
   do {
      Xstate last = currentState;
      int8_t tms = tapTransitions[currentState][to];
      if (tms < 0) {
         fprintf(fp_output, "(Failed)\n");
         return;
      }
      currentState = stateTable[currentState][tms];
      fprintf(fp_output, "->{%s,%d}", getXstateName(currentState),tms?'1':'0');
      if ((currentState == last) && (currentState != to)) {
         fprintf(fp_output, "(Failed - loop)\n");
         return;
      }
   } while (currentState != to);
   fprintf(fp_output, "\n");
}

/**
 * Get bits from  XSVF input
 *
 * @param title   Title for debug
 * @param count   Number if bits (rounded up to multiple of 8)
 * @param buff    Buffer for data
 */
void XsvfParser::getBits(unsigned count, uint8_t buff[MAX_BYTES]) {
   unsigned byteCount = (count+7)/8;
   assert(byteCount <= MAX_BYTES);
   unsigned index = 0;
   while(byteCount-->0) {
      buff[index++] = get();
   }
}

/**
 * Parse a single XSVF command
 *
 * @return false => Sequence not complete
 * @return false => Sequence completed (XCOMPLETE or no bytes left)
 */
bool XsvfParser::parse() {
   uint32_t value;
   XsvfCommand command = (XsvfCommand)get1Byte();

   fprintf(fp_output, "   /* %-10s      */", getXstateName(currentState));
   //      fprintf(fp_output, "   /* %10d      */", byteCounter);

   switch(command) {
      case XCOMPLETE   :
         /*
          * End of sequence
          */
         fprintf(fp_output, " XCOMPLETE,\n");
         return true;

      case XREPEAT :
         /*
          * XREPEAT times<1 byte>
          * XREPEAT defines the number of times that TDO is tested against the expected value
          * before the ISP operation is considered a failure.
          *
          * NOT IMPLEMENTED - IGNORED
          */
         repeat_count = get1Byte();
         fprintf(fp_output, " XREPEAT, %d,\n", repeat_count);
         break;

      case XSETSDRMASKS :
         /*
          * XSETSDRMASKS addressMask<"length" bits> dataMask<"length" bits>
          *
          * NOT IMPLEMENTED - ERROR
          */
         error = "XSETSDRMASKS not supported";
         fprintf(fp_output, "XSETSDRMASKS not supported\n");
         assert(0);
         break;

      case XSDRINC :
         /*
          * XSDRINC startAddress<"length" bits> numTimes<1 byte>
          *         data[1]<"length2" bits> ...data[numTimes]<"length2" bits>
          *
          * NOT IMPLEMENTED - ERROR
          */
         error = "XSDRINC not supported";
         fprintf(fp_output, "XSDRINC not supported\n");
         assert(0);
         break;

      case XRUNTEST :
         /*
          * XRUNTEST number<4 bytes>
          *
          * When the current XENDDR or XENDIR state is Run-Test/Idle (see XENDDR and XENDIR
          * commands), XRUNTEST defines the minimum number of TCK cycles to be applied in the
          * Run-Test/Idle state and the minimum number of microseconds the device should be in the
          * Run-Test/Idle state after each visit to the SDR (or SIR) state.
          * The single XRUNTEST number is a specification for both the minimum number of TCK cycles and
          * minimum number of microseconds to wait.
          * After transitioning from SDR (or SIR) to the Run-Test/Idle state, the
          * XSVF interpreter must apply the minimum number of TCK cycles and also must stay within the
          * Run-Test/Idle state for a minimum number of microseconds as specified by the last
          * XRUNTEST command.
          * If no prior XRUNTEST command exists, then the XSVF interpreter assumes an initial XRUNTEST number of zero.
          */
         run_test_time = get4Bytes();
         fprintf(fp_output, " XRUNTEST, BYTES32(%d), // cycle/ms\n", run_test_time);
         break;

      case XSDRSIZE :
         /*
          * XSDRSIZE length<4 bytes>
          *
          * Specifies the length of all XSDR/XSDRTDO records that follow.
          */
         xsdr_size = get4Bytes();
         fprintf(fp_output, " XSDRSIZE, BYTES32(%d),\n", xsdr_size);
         break;

      case XTDOMASK :
         /*
          * XTDOMASK value<"length" bits>
          *
          * Set TDO mask
          */
         fprintf(fp_output, " XTDOMASK,\n");
         getBits(xsdr_size, tdo_mask);
         printBits("tdo_mask", xsdr_size, tdo_mask);
         break;

      case XSTATE :
         /*
          * XSTATE state<1 byte>
          *
          * If the state is Test-Logic-Reset, then force the TAP to the Test-Logic-Reset state via the
          * guaranteed TAP reset sequence: hold TMS High and apply a minimum of five TCK cycles. For
          * non-zero state values, if the TAP is already in the specified state, then do nothing. Otherwise,
          * transition the TAP to the next specified state.
          */
      {
         Xstate state = static_cast<Xstate>(get1Byte());
         fprintf(fp_output, " XSTATE, %s,", getXstateName(state));
         moveTo(state, true);
         fprintf(fp_output, "\n");
      }
      break;

      case XENDIR :
         /*
          * XENDIR state<1 byte>
          *
          * Set the XSIR end state to Run-Test/Idle or Pause-IR.
          * The default is Run-Test/Idle.
          */
         value = get1Byte();
         endir_state = value?IR_Pause:Idle;
         fprintf(fp_output, " XENDIR, %s,\n", value?"Ex_IR_Pause":"Ex_Idle");
         break;

      case XENDDR :
         /*
          * XENDDR state<1 byte>
          *
          * Set the XSDR and XSDRTDO end state to Run-Test/Idle or Pause-DR.
          * The default is Run-Test/Idle.
          */
         value = get1Byte();
         enddr_state = value?DR_Pause:Idle;
         fprintf(fp_output, " XENDDR, %s,\n", value?"Ex_DR_Pause":"Ex_Idle");
         break;

      case XCOMMENT :
         /*
          * XCOMMENT char-string-ending-in-zero
          *
          * The XCOMMENT command specifies an arbitrary length character string that ends with a zero byte.
          */
      {
         fprintf(fp_output, " XCOMMENT\n");
         unsigned length = getString();
         fprintf(fp_output, "/*\n%s\n*/", string_buffer);
         printBits("XCOMMENT", length, reinterpret_cast<unsigned char *>(string_buffer));
      }
      break;

      case XWAIT :
         /*
          * XWAIT wait_state<1 byte> end_state<1 byte> wait_time<4 bytes>
          *
          * Go to the TAP wait_state, stay in the TAP wait_state for a minimum of wait_time (microseconds),
          * and finally go to the TAP end_state to complete the command.
          */
      {
         // State to wait in
         Xstate      wait_state = static_cast<Xstate>(get1Byte());
         // State to move to afterwards
         Xstate      end_state  = static_cast<Xstate>(get1Byte());
         // Time to spend in wait_state
         uint32_t    wait_time  = get4Bytes();

         fprintf(fp_output, " XWAIT, %s, %s, BYTES32(%d), // ",
               getXstateName(wait_state), getXstateName(end_state), wait_time);
         moveTo(wait_state, false);
         fprintf(fp_output, "-Wait(%d)", wait_time);
         moveTo(end_state, false);
         fprintf(fp_output, "\n");
      }
      break;

      case XSIR :
         /*
          * XSIR length<1 byte> TDIValue<"length" bits>
          *
          * Go to the Shift-IR state and shift in the TDIValue. If the last XRUNTEST time is non-zero, go to
          * the Run-Test/Idle state and wait for the last specified XRUNTEST time. Otherwise, go to the
          * last specified XENDIR state.
          */
         value = get1Byte();
         fprintf(fp_output, " XSIR, %d, // ", value);
         getBits(value, tdi_value);
         moveTo(IR_Shift);
         shiftIn(value, tdi_value, true);
         if (run_test_time != 0) {
            moveTo(Idle);
            fprintf(fp_output, "-Wait(%d)", run_test_time);
         }
         else {
            moveTo(endir_state);
            if (endir_state == IR_Pause) {
               assert((currentState == IR_Pause));
            }
         }
         fprintf(fp_output, "\n");
         if (value == 8) {
            bool doTrailer = true;
            fprintf(fp_output, "   /* tdi_value     8:*/ ");
            switch(tdi_value[0]) {
               case PROGRAM_COMMAND : fprintf(fp_output, "PROGRAM_COMMAND,");       break;
               case IDCODE_COMMAND  : fprintf(fp_output, "IDCODE_COMMAND,");       break;
               case BYPASS_COMMAND  : fprintf(fp_output, "BYPASS_COMMAND,");       break;
               case ENABLE_COMMAND  : fprintf(fp_output, "ENABLE_COMMAND,");       break;
               case ERASE_COMMAND   : fprintf(fp_output, "ERASE_COMMAND,");        break;
               case INIT_COMMAND    : fprintf(fp_output, "INIT_COMMAND,");         break;
               case CONLD_COMMAND   : fprintf(fp_output, "CONLD_COMMAND,");        break;
               case VERIFY_COMMAND  : fprintf(fp_output, "VERIFY_COMMAND,");       break;
               default              :        
                  fprintf(fp_output, "0x%02X,\n", tdi_value[0]);
                  doTrailer = false;
                  break;
            }
            if (doTrailer) {
               fprintf(fp_output, " // 0x%02X\n", tdi_value[0]);
            }
         }
         else {
            printBits("tdi_value", value, tdi_value);
         }
         break;

      case XSIR2 :
         /*
          * XSIR2 length<2 bytes> TDIValue<"length" bits>
          *
          * Go to the Shift-IR state and shift in the TDIValue. If the last XRUNTEST time is non-zero, go to
          * the Run-Test/Idle state and wait for the last specified XRUNTEST time. Otherwise, go to the
          * last specified XENDIR state.
          */
         value = get2Bytes();
         fprintf(fp_output, " XSIR2, %d, // ", value);
         getBits(value, tdi_value);
         moveTo(IR_Shift);
         shiftIn(value, tdi_value, true);
         if (run_test_time != 0) {
            moveTo(Idle);
            fprintf(fp_output, "-Wait(%d)", run_test_time);
         }
         else {
            moveTo(endir_state);
         }
         fprintf(fp_output, "\n");
         printBits("tdi_value", value, tdi_value);
         break;

      case XSDR :
         /*
          * XSDR TDIValue<"length" bits>
          *
          * Go to the Shift-DR state and shift in TDIValue; compare the TDOExpected value from the last
          * XSDRTDO instruction against the TDO value that was shifted out (use the TDOMask that was
          * generated by the last XTDOMASK instruction). Length comes from the XSDRSIZE instruction.
          */
         fprintf(fp_output, " XSDR, // ");
         getBits(xsdr_size, tdi_value);
         moveTo(DR_Shift);
         shiftIn(xsdr_size, tdi_value, tdo_value, tdo_mask, true);
         if (run_test_time != 0) {
            moveTo(Idle);
            fprintf(fp_output, "-Wait(%d)", run_test_time);
         }
         else {
            moveTo(endir_state);
         }
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         break;

      case XSDRTDO :
         /*
          * TDIValue<"length" bits> TDOExpected<"length" bits>
          *
          * Go to the Shift-DR state and shift in TDIValue; compare the TDOExpected value against the
          * TDO value that was shifted out (use the TDOMask which was generated by the last
          * XTDOMASK instruction). Length comes from the XSDRSIZE instruction.
          * If the TDO value does not match TDOExpected, perform the exception-handling sequence
          * described in the XC9500 programming algorithm section. If TDO is wrong more than the
          * maximum number of times specified by the XREPEAT instruction, then the ISP operation is
          * determined to have failed.
          * If the last XRUNTEST time is zero, then go to XENDDR state. Otherwise, go to the
          * Run_Test/Idle state and wait for the XRUNTEST time.
          * The TDOExpected Value is used in all successive XSDR instructions until the next XSDR
          * instruction is given.
          */
         fprintf(fp_output, " XSDRTDO, // ");
         getBits(xsdr_size, tdi_value);
         getBits(xsdr_size, tdo_value);
         moveTo(DR_Shift);
         shiftIn(xsdr_size, tdi_value, tdo_value, tdo_mask, true);
         if (run_test_time != 0) {
            moveTo(Idle);
            fprintf(fp_output, "-Wait(%d)", run_test_time);
         }
         else {
            moveTo(enddr_state);
         }
         fprintf(fp_output, "\n");
//         printBitsAsComment("tdi_value", xsdr_size, tdi_value);
         printBits("tdi_value", xsdr_size, tdi_value);
         printBits("tdo_value", xsdr_size, tdo_value);
         break;

      case XSDRB :
         /*
          * XSDRB TDIValue<"length" bits>
          *
          * Go to the shift-DR state and shift in the TDI value. Continue to stay in the shift-DR state at the end
          * of the operation. No comparison of TDO value with the last specified TDOExpected is performed.
          */
         fprintf(fp_output, " XSDRB, // ");
         getBits(xsdr_size, tdi_value);
         moveTo(DR_Shift);
         shiftIn(xsdr_size, tdi_value, false);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         break;

      case XSDRC :
         /*
          * XSDRC TDIValue<"length" bits>
          *
          * Shift in the TDI value. Continue to stay in the shift-DR state at the end of the operation. No
          * comparison of TDO value with the last specified TDOExpected is performed.
          */
         fprintf(fp_output, " XSDRC, // ");
         getBits(xsdr_size, tdi_value);
         shiftIn(xsdr_size, tdi_value, false);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         break;

      case XSDRE :
         /*
          * XSDRE TDIValue<"length" bits>
          *
          * Shift in the TDI value. At the end of the operation, go to the XENDDR state. No comparison of
          * TDO value with the last specified TDOExpected is performed.
          */
         fprintf(fp_output, " XSDRE, // ");
         assert(currentState==DR_Shift);
         getBits(xsdr_size, tdi_value);
         shiftIn(xsdr_size, tdi_value, true);
         moveTo(enddr_state);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         break;

      case XSDRTDOB :
         /*
          * XSDRTDOB TDIValue<"length" bits> TDOExpected<"length" bits>
          *
          * Go to the shift-DR state and shift in TDI value; compare the TDOExpected value against the
          * TDO value that was shifted out. TDOMask is not applied. All bits of TDO are compared with the
          * TDOExpected. Length comes from the XSDRSIZE instruction.
          * Because this instruction is primarily meant for FPGAs, if the TDO value does not match
          * TDOExpected, the programming is stopped with an error message. At the end of the
          * operations, continue to stay in the SHIFT-DR state.
          */
         fprintf(fp_output, " XSDRTDOB, // ");
         getBits(xsdr_size, tdi_value);
         getBits(xsdr_size, tdo_value);
         moveTo(DR_Shift);
         shiftIn(xsdr_size, tdi_value, tdo_value, false);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         printBits("tdo_value", xsdr_size, tdo_value);
         break;

      case XSDRTDOC :
         /*
          * XSDRTDOC TDIValue<"length" bits> TDOExpected<"length" bits>
          *
          * Shift in the TDI value; compare the TDOExpected value against the TDO value that was shifted
          * out. Length comes from the XSDRSIZE instruction. TDOMask is not applied. All bits of TDO are
          * compared with the TDOExpected.
          * If the TDO value does not match TDOExpected, stop the programming operation with an error
          * message. At the end of the operation, continue to stay in the SHIFT-DR state.
          */
         fprintf(fp_output, " XSDRTDOC, // ");
         getBits(xsdr_size, tdi_value);
         getBits(xsdr_size, tdo_value);
         assert(currentState==DR_Shift);
         shiftIn(xsdr_size, tdi_value, tdo_value, false);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         printBits("tdo_value", xsdr_size, tdo_value);
         break;

      case XSDRTDOE :
         /*
          * XSDRTDOE TDIValue<"length" bits> TDOExpected<"length" bits>
          *
          * Shift in the TDI value; compare the TDOExpected value against the TDO value that was shifted
          * out. Length comes from the last XSDSIZE instruction. TDOMask is not applied. All bits of TDO
          * are compared with the TDOExpected.
          * If the TDO value does not match the TDOExpected, stop the programming operations with an
          * error message. At the end of the operation, go to the XENDDR state.
          */
         fprintf(fp_output, " XSDRTDOC, // ");
         getBits(xsdr_size, tdi_value);
         getBits(xsdr_size, tdo_value);
         assert(currentState==DR_Shift);
         shiftIn(xsdr_size, tdi_value, tdo_value, true);
         moveTo(enddr_state);
         fprintf(fp_output, "\n");
         printBits("tdi_value", xsdr_size, tdi_value);
         printBits("tdo_value", xsdr_size, tdo_value);
         break;

      default :
         error = "Unknown command";
         return true;
   }
   return false;
}

/**
 * Parse XSVF sequence
 *
 * @return true  XSVF sequence completed without error
 * @return false Error detected during XSVF sequence
 */
bool XsvfParser::parseAll(unsigned xsvf_data_size) {
   std::string cppName(fileName);
   cppName += "_xsvf.cpp";

   std::string cppHeaderName(fileName);
   cppHeaderName += "_xsvf.h";

   fp_output = fopen(cppName.c_str(), "wt");
   if (fp_output == nullptr) {
      fprintf(stderr, "Creating %s failed\n", cppName.c_str());
      return false;
   }
   fprintf(fp_output, "// Created from : \'%s\'\n\n", fileName.c_str());

   FILE *fp_header = fopen(cppHeaderName.c_str(), "wt");
   if (fp_output == nullptr) {
      fprintf(stderr, "Creating %s failed\n", cppHeaderName.c_str());
      return false;
   }
   fprintf(fp_header,
         "/*\n"
         " * %s\n"
         " *\n"
         " *  Created on: 16 Dec 2019\n"
         " *      Author: podonoghue\n"
         " */\n"
         "\n"
         "#ifndef SOURCES_%s_xsvf_H_\n"
         "#define SOURCES_%s_xsvf_H_\n"
         "\n"
         "#include <stdint.h>\n"
         "\n"
         "extern const uint8_t %s_xsvf[%u];\n"
         "\n"
         "#endif /* SOURCES_%s_xsvf_H_ */\n",
         fileName.c_str(), fileName.c_str(), fileName.c_str(), fileName.c_str(), xsvf_data_size, fileName.c_str()
   );
   fclose(fp_header);

   static const char header[] =
         "#include <stdint.h>\n"
         "#include \"%s\"\n"
         "#include \"JtagTables.h\"\n"
         "\n"
         "/// Command codes               \n"
         "enum Command {                  \n"
         "   XCOMPLETE    = 0x00,         \n"
         "   XTDOMASK     = 0x01,         \n"
         "   XSIR         = 0x02,         \n"
         "   XSDR         = 0x03,         \n"
         "   XRUNTEST     = 0x04,         \n"
         "   XREPEAT      = 0x07,         \n"
         "   XSDRSIZE     = 0x08,         \n"
         "   XSDRTDO      = 0x09,         \n"
         "   XSETSDRMASKS = 0x0a,         \n"
         "   XSDRINC      = 0x0b,         \n"
         "   XSDRB        = 0x0c,         \n"
         "   XSDRC        = 0x0d,         \n"
         "   XSDRE        = 0x0e,         \n"
         "   XSDRTDOB     = 0x0f,         \n"
         "   XSDRTDOC     = 0x10,         \n"
         "   XSDRTDOE     = 0x11,         \n"
         "   XSTATE       = 0x12,         \n"
         "   XENDIR       = 0x13,         \n"
         "   XENDDR       = 0x14,         \n"
         "   XSIR2        = 0x15,         \n"
         "   XCOMMENT     = 0x16,         \n"
         "   XWAIT        = 0x17,         \n"
         "};                              \n"
         "\n"
         "#define BYTES32(x) ((uint8_t)((x)>>24)),((uint8_t)((x)>>16)),((uint8_t)((x)>>8)),((uint8_t)(x))\n"
         "\n"
         "static constexpr uint8_t Ex_Idle         = 0x00;\n"
         "static constexpr uint8_t Ex_DR_Pause     = 0x01;\n"
         "static constexpr uint8_t Ex_IR_Pause     = 0x01;\n"
         "static constexpr uint8_t IDCODE_COMMAND  = 0x01;\n"
         "static constexpr uint8_t BYPASS_COMMAND  = 0xFF;\n"
         "static constexpr uint8_t ENABLE_COMMAND  = 0xE8;\n"
         "static constexpr uint8_t ERASE_COMMAND   = 0xED;\n"
         "static constexpr uint8_t INIT_COMMAND    = 0xF0;\n"
         "static constexpr uint8_t CONLD_COMMAND   = 0xC0;\n"
         "static constexpr uint8_t VERIFY_COMMAND  = 0xEE;\n"
         "static constexpr uint8_t PROGRAM_COMMAND = 0xEA;\n"
         "\n"
         "const uint8_t %s_xsvf[%u] = {\n"
         ;

   static const char *trailer =
         "};";

   fprintf(fp_output, header, cppHeaderName.c_str(), fileName.c_str(), xsvf_data_size);

   while (!parse()) {
   }

   const char *errorMessage = getError();
   fprintf(fp_output, "// Processed %d bytes\n", byteCounter);
   if (errorMessage != nullptr) {
      fprintf(fp_output, "// Error = %s", errorMessage);
   }
   // Move to EOF
   fprintf(fp_output, trailer);

   return false;
}


/**
 * Get single byte from XSVF input
 *
 * @return Byte obtained
 */
uint8_t XsvfParser_File::get() {
   if (byteCounter>xsvf_data_size) {
      return XCOMPLETE;
   }
   byteCounter++;
   return fgetc(fp_input);
}

