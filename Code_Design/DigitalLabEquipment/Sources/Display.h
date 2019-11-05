/*
 * Display.h
 *
 *  Created on: 5 Sep 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_DISPLAY_H_
#define SOURCES_DISPLAY_H_

#include "Oled.h"

class FrequencyDisplay {

private:
   USBDM::Oled &oled;

   void drawDigit(int value, bool &suppress);

public:
   FrequencyDisplay(USBDM::Oled &oled) : oled(oled) {
   }

   /**
    * Display Frequency on LCD
    *
    * @param frequency
    */
   void displayFrequency(unsigned frequency);

};

#endif /* SOURCES_DISPLAY_H_ */
