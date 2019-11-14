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
#include "FunctionQueue.h"

class FrequencyGenerator {

private:
   static FrequencyGenerator *This;

   USBDM::Oled &oled;
   unsigned savedFrequency   = Frequency_Off;
   unsigned currentFrequency = Frequency_Off;
   FunctionQueue &functionQueue;

   /**
    * Display Frequency on OLED
    *
    * @param frequency
    */
   void displayFrequency(unsigned frequency);
   void initialiseWaveform();
   /**
    * Set frequency of generator
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
   FrequencyGenerator(USBDM::Oled &oled, FunctionQueue &functionQueue) : oled(oled), functionQueue(functionQueue) {
      usbdm_assert((This == nullptr), "Only single instance of FrequencyGenerator allowed");
      This = this;
      initialiseWaveform();
      setFrequency(Frequency_Off);
      displayFrequency(currentFrequency);
   }

   void pollButtons();

   void softPowerOn();

   void softPowerOff();

   /**
    * Test loop for debug
    */
   void testLoop();
};

#endif /* SOURCES_FREQUENCYGENERATOR_H_ */
