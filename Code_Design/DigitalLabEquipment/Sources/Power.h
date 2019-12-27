/*
 * Power.h
 *
 *  Created on: 14 Nov 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_POWER_H_
#define SOURCES_POWER_H_

#include "Configure.h"

class Power;

class PowerSubscriber {

private:
   friend Power;

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn()  = 0;

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() = 0;

public:

   PowerSubscriber(){}
   virtual ~PowerSubscriber(){}
};

class Power {
private:
   bool     powerOn              = false;
   bool     lastButtonValue      = false;
   unsigned stableCount          = 0;
   unsigned powerOnRecoveryCount = 0;

   PowerSubscriber *powerSubscribers[5];
   unsigned         powerSubscriberCount = 0;

   static constexpr USBDM::AdcResolution adcResolution = USBDM::AdcResolution_8bit_se;

   /**
    * Notifies all devices that soft power-on has occurred
    *
    * @note This is done after the power-on
    */
   void powerOnNotify() {
      for (unsigned index=0; index<powerSubscriberCount; index++) {
         powerSubscribers[index]->softPowerOn();
      }
   }

   /**
    * Notifies all devices that soft power-off has occurred
    *
    * @note This is done before the power-off
    */
   void powerOffNotify() {
      for (unsigned index=0; index<powerSubscriberCount; index++) {
         powerSubscribers[index]->softPowerOff();
      }
   }

   // Self pointer for static methods e.g. call-backs
   static Power    *This;

   static void callback(uint32_t result, int channel) {
      This->vddSampleCallback(result, channel);
   }

   /**
    * Initialise Power object
    */
   void powerInitialise();

   /**
    * ADC Callback used to monitor overloads
    *
    * @param result
    * @param channel
    */
   void vddSampleCallback(uint32_t result, int channel);

   /**
    * Enable target Vdd
    *
    * Disconnects TargetVddDischarge
    * Connects TargetVddSample to target Vdd
    */
   void enableTargetVdd() {
      TargetVddDischarge::off();
      TargetVddSample::setInput();
      PowerEnableControl::on();
   }

   /**
    * Disable target Vdd
    *
    * Connects TargetVddDischarge to target Vdd and makes active
    * Disconnects TargetVddSample
    */
   void disableTargetVdd() {
      PowerEnableControl::off();
      TargetVddDischarge::setOutput(USBDM::PinDriveStrength_High, USBDM::PinDriveMode_OpenDrain, USBDM::PinSlewRate_Slow);
      TargetVddDischarge::on();
   }

public:
   Power() {

      usbdm_assert((This == nullptr), "Only one instance of Power object is allowed");

      This = this;
      powerInitialise();
   }

   /**
    * Add an object to be notified of soft power-on and power-off events
    *
    * @param powerSubscriber
    */
   void addPowerSubscriber(PowerSubscriber *powerSubscriber) {
      powerSubscribers[powerSubscriberCount++] = powerSubscriber;
   }

   /**
    * Poll power button and monitor target Vdd
    */
   void pollPower();

   /**
    * Indicates if the power is current on
    *
    * @return  True  => power is on
    * @return  False => power is off
    */
   bool isPowerOn() {
      return powerOn;
   }
};

#endif /* SOURCES_POWER_H_ */
