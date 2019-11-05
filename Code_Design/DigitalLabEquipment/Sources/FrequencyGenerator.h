/*
 * waveform.h
 *
 *  Created on: 5 Sep 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_FREQUENCYGENERATOR_H_
#define SOURCES_FREQUENCYGENERATOR_H_

#include "hardware.h"
#include "Oled.h"
#include "Configure.h"

class FrequencyGenerator {

private:
   USBDM::Oled &oled;
   unsigned currentFrequency = 100 * USBDM::Hz;

   /**
    * Display Frequency on LCD
    *
    * @param frequency
    */
   void displayFrequency(unsigned frequency);
   void initialiseWaveform();

public:
   /// Used to indicate generator off
   static constexpr unsigned Frequency_Off = 0.0;

   /// Minimum frequency available
   static constexpr unsigned Frequency_Min = 1 * USBDM::Hz;

   /// Maximum frequency available
   static constexpr unsigned Frequency_Max = 10 * USBDM::MHz;

   /**
    * Construct Frequency generator
    *
    * @param oled  OLED display to use to display frequency
    */
   FrequencyGenerator(USBDM::Oled &oled) : oled(oled) {
      initialiseWaveform();
   }

   /**
    * Set frequency of generator
    * This will also update the OLED display
    *
    * @param frequency
    *
    * @note Range: Frequency_Off, [Frequency_Min, Frequency_Max]
    */
   void setFrequency(unsigned frequency);

   /**
    * Get the current waveform frequency
    *
    * @return Frequency in Hz
    */
   unsigned getFrequency();

   /**
    * Test loop for debug
    */
   void testLoop();
};

#endif /* SOURCES_FREQUENCYGENERATOR_H_ */
