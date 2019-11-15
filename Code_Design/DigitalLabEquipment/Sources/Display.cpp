/*
 * Display.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: podonoghue
 */

#include <string.h>

#include "Display.h"

#include "Oled.h"

// Access to USBDM name-space
using namespace USBDM;

/**
 * Display Frequency on LCD
 *
 * @param frequency
 */
void FrequencyDisplay::displayFrequency(unsigned frequency) {
   const char *units = "    ";

   if (frequency >= 1*MHz) {
      units = " MHz";
      frequency /= 1000000;
   }
   else if (frequency >= 1*kHz) {
      units = " kHz";
      frequency /= 1000;
   }
   else if (frequency != 0) {
      units = "  Hz";
   }

   oled.clearDisplay();
   oled.setFont(fontVeryLarge);

   bool suppressZeroes = true;
   auto drawDigit = [&](int value) {
      suppressZeroes = suppressZeroes && (value == 0);
      if (suppressZeroes) {
         oled.write(' ');
      }
      else {
         oled.write(value);
      }
   };

   drawDigit(frequency/100);
   drawDigit((frequency/10)%10);
   drawDigit(frequency%10);

   oled.write(units).write(" ");

   oled.refreshImage();
}
