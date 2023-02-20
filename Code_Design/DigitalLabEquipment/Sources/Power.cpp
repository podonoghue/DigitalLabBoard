/*
 * Power.cpp
 *
 *  Created on: 14 Nov 2019
 *      Author: podonoghue
 */

#include "Power.h"

using namespace USBDM;

/**
 * Initialise Power object
 */
void Power::powerInitialise() {
   PowerButton::setInput(PinPull_Up, PinAction_None, PinFilter_None);
   PowerEnableControl::setOutput(PinDriveStrength_High, PinDriveMode_PushPull, PinSlewRate_Slow);

   disableTargetVdd();

   Adc0::setCallback(callback);
   Adc0::enableNvicInterrupts(NvicPriority_Normal);
}

/**
 * Poll power button and monitor target Vdd
 */
void Power::pollPower() {
   if (powerOnRecoveryCount>0) {
      powerOnRecoveryCount--;
   }
   bool currentButtonValue = PowerButton::read();

   if (lastButtonValue != currentButtonValue) {
      stableCount     = 0;
      lastButtonValue = currentButtonValue;
      return;
   }
   if (stableCount < POWER_BUTTON_DELAY_COUNT+1) {
      stableCount++;
   }
   if (stableCount == POWER_BUTTON_DELAY_COUNT) {
      if (currentButtonValue) {
         powerOn = !powerOn;
         if (powerOn) {
            enableTargetVdd();
            powerOnRecoveryCount = POWER_ON_DELAY_COUNT;
         }
         else {
            powerOffNotify();

            static auto f = []() {
               // Disable I2C pins as power cycle can upset SCL/SDA
               I2cInterface::disableAllPins();
               disableTargetVdd();
               return USBDM::E_NO_ERROR;
            };
            // Schedule power off (after pending transfers)
            functionQueue.enQueueDiscardOnFull(f);
         }
      }
   }
   if ((powerOnRecoveryCount <= (POWER_ON_DELAY_COUNT-POWER_ON_ADC_DELAY_COUNT)) && powerOn && !Adc0::isBusy()) {
      Adc0::configure(adcResolution, AdcClockSource_Bus);
      TargetVddSample::startConversion(AdcInterrupt_Enabled);
   }
   if (powerOnRecoveryCount == 1) {
      // Enable I2C pins after power stable
      I2cInterface::configureAllPins();
      powerOnNotify();
   }
}

/**
 * ADC Callback used to monitor overloads
 *
 * @param result
 * @param channel
 */
void Power::vddSampleCallback(uint32_t result, int) {
   if (powerOn && (result<(Adc0::getSingleEndedMaximum(adcResolution)*0.8))) {
      powerOffNotify();
      disableTargetVdd();
      powerOn = false;
   }
}

Power *Power::This    = nullptr;
