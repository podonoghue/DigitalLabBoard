/*
 ============================================================================
 * @file    xsvf_Parse.cpp
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "xsvfParser.h"

#if 0
static void listPaths() {
   // List stable-state transitions
   Xsvf::printTransition(Reset, Reset);
   Xsvf::printTransition(Reset, Idle);
   Xsvf::printTransition(Reset, DR_Pause);
   Xsvf::printTransition(Reset, IR_Pause);

   Xsvf::printTransition(Idle, Reset);
   Xsvf::printTransition(Idle, Idle);
   Xsvf::printTransition(Idle, DR_Pause);
   Xsvf::printTransition(Idle, IR_Pause);

   Xsvf::printTransition(DR_Pause, Reset);
   Xsvf::printTransition(DR_Pause, Idle);
   Xsvf::printTransition(DR_Pause, DR_Pause);
   Xsvf::printTransition(DR_Pause, IR_Pause);

   Xsvf::printTransition(IR_Pause, Reset);
   Xsvf::printTransition(IR_Pause, Idle);
   Xsvf::printTransition(IR_Pause, DR_Pause);
   Xsvf::printTransition(IR_Pause, IR_Pause);

   Xsvf::// List consecutive state transitions
   Xsvf::printTransition(Reset,     Idle);
   Xsvf::printTransition(Reset,     Reset);
   Xsvf::printTransition(Idle,      Idle);
   Xsvf::printTransition(Idle,      DR_Select);
   Xsvf::printTransition(DR_Select, DR_Capture);
   Xsvf::printTransition(DR_Select, IR_Select);
   Xsvf::printTransition(DR_Capture,DR_Shift);
   Xsvf::printTransition(DR_Capture,DR_Exit1);
   Xsvf::printTransition(DR_Shift,  DR_Shift);
   Xsvf::printTransition(DR_Shift,  DR_Exit1);
   Xsvf::printTransition(DR_Exit1,  DR_Pause);
   Xsvf::printTransition(DR_Exit1,  DR_Update);
   Xsvf::printTransition(DR_Pause,  DR_Pause);
   Xsvf::printTransition(DR_Pause,  DR_Exit2);
   Xsvf::printTransition(DR_Exit2,  DR_Shift);
   Xsvf::printTransition(DR_Exit2,  DR_Update);
   Xsvf::printTransition(DR_Update, Idle);
   Xsvf::printTransition(DR_Update, DR_Select);

   Xsvf::printTransition(IR_Select, IR_Capture);
   Xsvf::printTransition(IR_Select, Reset);
   Xsvf::printTransition(IR_Capture,IR_Shift);
   Xsvf::printTransition(IR_Capture,IR_Exit1);
   Xsvf::printTransition(IR_Shift,  IR_Shift);
   Xsvf::printTransition(IR_Shift,  IR_Exit1);
   Xsvf::printTransition(IR_Exit1,  IR_Pause);
   Xsvf::printTransition(IR_Exit1,  IR_Update);
   Xsvf::printTransition(IR_Pause,  IR_Pause);
   Xsvf::printTransition(IR_Pause,  IR_Exit2);
   Xsvf::printTransition(IR_Exit2,  IR_Shift);
   Xsvf::printTransition(IR_Exit2,  IR_Update);
   Xsvf::printTransition(IR_Update, Idle);
   Xsvf::printTransition(IR_Update, DR_Select);
}
#endif

int main(int argc, const char *argv[]) {
   const char *filename = "cpld_tester";
   if (argc>1) {
      filename = argv[1];
   }
   XsvfParser_File xsvf(filename);
   xsvf.parseAll();
   return 0;
}
