/*
 * SwitchDebouncer.cpp
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */
#include "Switches.h"

   /// Self pointer for static methods e.g. call-backs
Switches *Switches::This = nullptr;

/**
 * Notification that soft power-on has occurred
 */
void Switches::softPowerOn() {

   powerOn = true;

   // User outputs are always output
   switchGpio.setDirection0(0b11111111);

   // Update user outputs
   switchGpio.writeData0(currentOutputValues|latchedOutputValues);

   // Data is always 0 (pins are low/high-z)
   switchGpio.writeData1(0b00000000);

   // Restore LEDs (either 3-state or low)
   switchGpio.setDirection1(currentOutputValues|latchedOutputValues);
}

/**
 * Notification that soft power-off is about to occur
 */
void Switches::softPowerOff() {

   powerOn = false;
}

void Switches::runTests() {
   static uint8_t data = 0;

   switchGpio.writeData0(0b00000000);
   data = ~data;
   switchGpio.setDirection1(data);
}

/**
 * Serialised call-back function
 * - Polls switches
 * - Updates LEDs and user outputs
 *
 * Switches and LEDs share the same pin of the GPIO expander.
 * The port is 3-stated to read the switches (using internal GPIO pull-ups).
 * Polled switches are active-low.
 *
 * After polling the GPIO is driven low or left 3-state to control the LEDs,
 * i.e. data value is always 0 and direction is controlled based on switch value.
 */
void Switches::updateSwitches() {
   static uint8_t  lastButtonValue      = 0;
   static unsigned stableCount          = 0;

   if (!powerOn) {
      // Don't access peripherals if no power!
      return;
   }

   // Make Button GPIOs inputs
   switchGpio.setDirection1(0b00000000);

   // Poll switches - active low
   uint8_t  currentButtonValue   = 0;
   switchGpio.readData1(currentButtonValue);
   currentButtonValue = ~currentButtonValue;

   if (lastButtonValue != currentButtonValue) {
      stableCount     = 0;
      lastButtonValue = currentButtonValue;
   }
   else {
      if (stableCount < LATCHED_INTERVAL_COUNT+1) {
         stableCount++;
      }
      if (stableCount == DEBOUNCE_INTERVAL_COUNT) {
         // Update switch values
         currentOutputValues = currentButtonValue;
         // Release latching on any active buttons
         latchedOutputValues &= ~currentButtonValue;
      }
      if (stableCount == LATCHED_INTERVAL_COUNT) {
         // Latch active buttons
         latchedOutputValues |= currentButtonValue;
      }
   }
   // Restore LEDs (either 3-state or low)
   switchGpio.setDirection1(currentOutputValues|latchedOutputValues);

   // Update switch outputs
   switchGpio.writeData0(currentOutputValues|latchedOutputValues);
}
