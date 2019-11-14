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

class Switches {
private:
   static Switches *This;

   USBDM::Pca9555 switchGpio;
   Power          &power;

   FunctionQueue  &functionQueue;

   static void updateSwitches();

public:
   Switches(USBDM::I2c &i2c, Power &power, FunctionQueue &functionQueue);

   uint8_t data = 0;

   void runTests();

   void pollButtons() {
      functionQueue.enQueueDiscardOnFull(updateSwitches);
   }
};


#endif /* SOURCES_SWITCHES_H_ */
