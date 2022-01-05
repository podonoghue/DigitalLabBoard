/*
 * Traffic.cpp
 *
 *  Created on: 15 Nov 2019
 *      Author: podonoghue
 */
#include "Traffic.h"

using namespace USBDM;

/**
 * Serialised call-back function
 * - Polls switches and user inputs
 * - Updates LEDs and user outputs
 */
void Traffic::updateSwitches() {
   static uint8_t  lastButtonValue      = 0;
   static unsigned stableCount          = 0;

   if (!powerOn) {
      // Don't access peripherals if no power!
      return;
   }

   uint8_t currentButtonValue = TrafficButtons::read();

   if (lastButtonValue != currentButtonValue) {
      stableCount     = 0;
      lastButtonValue = currentButtonValue;
   }
   else {
      if (stableCount < DEBOUNCE_INTERVAL_COUNT+1) {
         stableCount++;
      }
      if (stableCount == DEBOUNCE_INTERVAL_COUNT) {
         // Update switch outputs
         trafficGpio.writeData0(currentButtonValue<<4);

         // Enable interface on 1st button press
         enabled = enabled || (currentButtonValue != 0b0000);
      }
   }
   if (!enabled) {
      return;
   }

   uint8_t lightInputs;
   trafficGpio.readData0(lightInputs);

   uint8_t ledMask = trafficLedsEncode(northSouthLights(lightInputs), eastWestLights(lightInputs));

   // Update LEDs
   trafficGpio.writeData1(ledMask);
}

/**
 * Notification that soft power-on has occurred
 */
void Traffic::softPowerOn() {
   // Initialise GPIO as state has been lost

   powerOn = true;

   // Buttons are shared so fully initialise each time
   TrafficButtons::setInput(
         PinPull_None,
         PinAction_None,
         PinFilter_None);

   // User I/O
   trafficGpio.setDirection0(0b11110000);
   trafficGpio.writeData0(0b0000);

   // LEDs - All off initially
   trafficGpio.setDirection1(0b11111111);
   trafficGpio.writeData1(0b00000000);
}

/**
 * Notification that soft power-off is about to occur
 */
void Traffic::softPowerOff() {

   powerOn = false;
}

/// Self pointer for static methods e.g. call-backs
Traffic *Traffic::This = nullptr;

