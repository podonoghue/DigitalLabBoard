/*
 * SwitchDebouncer.h
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_SWITCHES_H_
#define SOURCES_SWITCHES_H_

#include "hardware.h"
#include "i2c.h"
#include "PCA9555.h"
#include "Power.h"
#include "FunctionQueue.h"

class Switches : PowerSubscriber {

private:
   /// Self pointer for static methods e.g. call-backs
   static Switches *This;

   /// Current state of non-latched user outputs
   uint8_t  currentOutputValues  = 0;

   /// Current state of latched user outputs
   uint8_t  latchedOutputValues  = 0;

   /// Conflicted outputs
   uint8_t  conflictedOutputValues  = 0;

   /// GPIO used for shared LEDs/buttons pins and user outputs
   USBDM::Pca9555 switchGpio;

   /// Queue for serialised function execution
   FunctionQueue  &functionQueue;

   /// Whether interface is powered
   bool powerOn = false;

   /**
    * Serialised call-back function
    * - Polls switches
    * - Updates LEDs and user outputs
    */
   void updateSwitches();

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn() override;

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() override;

public:
   /**
    * Switch module
    *
    * @param i2c              I2C interface for GPIO port expander
    * @param functionQueue    Serialised call-back function queue
    * @param power            Power object
    */
   Switches(USBDM::I2c &i2c, FunctionQueue &functionQueue, Power &power) :
      switchGpio(USBDM::Pca9555(i2c,  BUTTON_I2C_ADDRESS)), functionQueue(functionQueue) {
      usbdm_assert((This == nullptr), "Only single instance of Switches allowed");
      This = this;

      power.addPowerSubscriber(this);
   }

   void runTests();

   /**
    * Schedules all polling operations
    */
   void pollButtons() {
      static auto f = []() {
         This->updateSwitches();
      };
      functionQueue.enQueueDiscardOnFull(f);
   }
};

#endif /* SOURCES_SWITCHES_H_ */
