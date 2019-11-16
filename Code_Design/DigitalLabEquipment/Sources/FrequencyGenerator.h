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
#include "Power.h"

class FrequencyGenerator : PowerSubscriber {

private:

   /// Self pointer for static methods e.g. call-backs
   static FrequencyGenerator *This;

   // OLED display to use
   USBDM::Oled &oled;

   // Saved frequency for power-off
   unsigned savedFrequency   = Frequency_Off;

   // Current output frequency
   unsigned currentFrequency = Frequency_Off;

   /// Queue for serialised function execution
   FunctionQueue &functionQueue;

   /**
    * Table of available frequencies
    */
   static const float freqs[];

   /**
    * Display Frequency on OLED
    *
    * @param frequency
    */
   void displayFrequency(unsigned frequency);

   /**
    * Initialise Clock generator
    */
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

   /// Whether interface is powered
   bool powerOn = false;

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn() override;

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() override;

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
   FrequencyGenerator(USBDM::Oled &oled, FunctionQueue &functionQueue, Power &power) : oled(oled), functionQueue(functionQueue) {
      usbdm_assert((This == nullptr), "Only single instance of FrequencyGenerator allowed");
      This = this;
      initialiseWaveform();
      setFrequency(Frequency_Off);
      power.addPowerSubscriber(this);
   }

   /**
    * Do all polling operations
    */
   void pollButtons();

   /**
    * Test loop for debug
    */
   void testLoop();
};

#endif /* SOURCES_FREQUENCYGENERATOR_H_ */
