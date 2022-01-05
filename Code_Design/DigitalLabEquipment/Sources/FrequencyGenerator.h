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
#include "FunctionQueue.h"
#include "Power.h"

class FrequencyGenerator : PowerSubscriber {

private:

   /// Self pointer for static methods e.g. call-backs
   static FrequencyGenerator *This;

   /// OLED display to use
   USBDM::Oled &oled;

   /// Saved frequency for power-off
   unsigned savedFrequency   = Frequency_Off;

   /// Current output frequency
   unsigned currentFrequency = Frequency_Off;

   /// Queue for serialised function execution
   FunctionQueue &functionQueue;

   /// Whether interface is powered
   bool powerOn = false;

   /// Optional message to display on initial power-up
   const char *startupMessage = nullptr;

   /**
    * Table of available frequencies
    */
   static const float freqs[];

   /**
 * Display current Frequency on LCD
 * This is done on the function queue
    */
   void refreshFrequency();

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

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn() override;

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() override;

   void ftmCallback(uint8_t status);

   void refreshOled() {
      // Updating OLED is time consuming and uses shared I2C so done on function queue
      static auto f = []() {
         This->oled.refreshImage();
      };
      functionQueue.enQueue(f);
   }

   void initialiseOled() {
      // Initialising OLED is time consuming and uses shared I2C so done on function queue
      static auto f = []() {
         This->oled.initialise();
      };
      functionQueue.enQueue(f);
   }

public:
   /// Used to indicate generator off
   static constexpr unsigned Frequency_Off = 0.0;

   /// Minimum frequency available
   static constexpr unsigned Frequency_Min = 1_Hz;

   /// Maximum frequency available
   static constexpr unsigned Frequency_Max = 10_MHz;

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

   /**
    * Display message on OLED
    *
    * @param msg Message to display
    *
    * @note This can only be done BEFORE the signal generator is enabled.
    */
   void setStartupMessage(const char *msg) {
      startupMessage = msg;
   }
};

#endif /* SOURCES_FREQUENCYGENERATOR_H_ */
