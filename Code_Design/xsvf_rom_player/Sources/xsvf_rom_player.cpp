/*
 ============================================================================
 * @file    xsvf_rom_player.cpp
 *
 * Stand-alone XSVF player
 * Plays a XSVF sequence hard-programmed in a C array.
 *
 *  Created on: 22 Dec 2019
 *      Author: podonoghue
 ============================================================================
 */
#include "cpld_tester_xsvf.h"
#include "hardware.h"
#include "xsvfPlayer.h"
#include "JtagInterface.h"

// Allow access to USBDM methods
using namespace USBDM;

using PassLED = GpioD<6>;
using FailLED = GpioD<7>;

// Create player
XsvfPlayer_Array xsvf{sizeof(cpld_tester_xsvf), cpld_tester_xsvf};

int main() {

   PassLED::setOutput(
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinSlewRate_Slow);

   FailLED::setOutput(
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinSlewRate_Slow);

   waitMS(100);

   console.writeln("Waiting for Vref");
   while (!JtagInterface::checkVref()) {
      __asm__("nop");
   }
   waitMS(100);
   console.writeln("Vref present - programming");

   FailLED::on();
   bool success = xsvf.playAll();
   FailLED::off();

   if (success) {
      // Success - Slow blinking
      console.writeln("Sequence completed");
      for(;;) {
         PassLED::toggle();
         waitMS(500);
      }
   }
   else {
      // Failure - Fast blinking
      console.writeln("Sequence failed");
      for(;;) {
         FailLED::toggle();
         waitMS(50);
      }
   }

   return 0;
}
