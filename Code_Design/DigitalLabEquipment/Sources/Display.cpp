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

void FrequencyDisplay::drawDigit(int value, bool &suppress) {
   suppress = suppress && (value == 0);
   if (suppress) {
      oled.write(' ');
   }
   else {
      oled.write(value);
   }
}

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
   bool suppressZeroes = true;

   oled.clearDisplay();
   oled.setFont(fontVeryLarge);

   drawDigit(frequency/100,      suppressZeroes);
   drawDigit((frequency/10)%10,  suppressZeroes);
   drawDigit(frequency%10,       suppressZeroes);

   oled.write(units).write(" ");

   oled.refreshImage();
}
