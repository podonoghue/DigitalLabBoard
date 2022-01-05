/*
 * Traffic.h
 *
 *  Created on: 15 Nov 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_TRAFFIC_H_
#define SOURCES_TRAFFIC_H_

#include "Power.h"
#include "PCA9555.h"
#include "FunctionQueue.h"

class Traffic : PowerSubscriber {
private:

   /// Self pointer for static methods e.g. call-backs
   static Traffic *This;

   /// GPIO used for LEDs and user I/O
   USBDM::Pca9555 trafficGpio;

   /// Queue for serialised function execution
   FunctionQueue  &functionQueue;

   /// Indicates if module is enabled
   bool enabled = false;

   /// Whether interface is powered
   bool powerOn = false;

   /**
    * Serialised call-back function
    * - Polls switches and user inputs
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
    * Traffic module
    *
    * @param i2c              I2C interface for GPIO port expander
    * @param functionQueue    Serialised call-back function queue
    * @param power            Power object
    */
   Traffic(USBDM::I2c &i2c, FunctionQueue &functionQueue, Power &power) :
      trafficGpio(USBDM::Pca9555(i2c,  TRAFFIC_I2C_ADDRESS)), functionQueue(functionQueue) {

      usbdm_assert((This == nullptr), "Only single instance of Traffic allowed");
      This = this;

      power.addPowerSubscriber(this);
   }

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

#endif /* SOURCES_TRAFFIC_H_ */
