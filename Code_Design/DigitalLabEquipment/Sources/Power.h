/*
 * Power.h
 *
 *  Created on: 14 Nov 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_POWER_H_
#define SOURCES_POWER_H_

#include "FrequencyGenerator.h"

class Power {
private:
   bool     powerOn              = false;
   bool     lastButtonValue      = false;
   unsigned stableCount          = 0;
   unsigned powerOnRecoveryCount = 0;


   static Power    *This;

   FrequencyGenerator &frequencyGenerator;

   static void callback(uint32_t result, int channel) {
      This->vddSampleCallback(result, channel);
   }

   void powerInitialise();

public:
   Power(FrequencyGenerator &frequencyGenerator) : frequencyGenerator(frequencyGenerator) {
      usbdm_assert((This == nullptr), "Only one instance of Power object is allowed");
      This = this;
      powerInitialise();
   }

   /**
    * Does recovery after soft power-on
    * This routine is called after a delay.
    */
   void powerOnRecovery();

   /**
    * Called to turn off all devices on soft power-off
    */
   void powerOff();

   /**
    * Poll power button and monitor target Vdd
    */
   void pollPower();

   /**
    * ADC Callback used to monitor overloads
    *
    * @param result
    * @param channel
    */
   void vddSampleCallback(uint32_t result, int channel);

   bool isPowerOn() {
      return powerOn;
   }
};

#endif /* SOURCES_POWER_H_ */
