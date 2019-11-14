/*
 * SwitchDebouncer.cpp
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */
#include <Switches.h>

Switches *Switches::This = nullptr;

Switches::Switches(USBDM::I2c &i2c, Power &power, FunctionQueue &functionQueue) :
   switchGpio(USBDM::Pca9555(i2c,  BUTTON_I2C_ADDRESS)), power(power), functionQueue(functionQueue) {
   usbdm_assert((This == nullptr), "Only single instance of Switches allowed");
   This = this;

   // Always output
   switchGpio.setDirection0(0b11111111);
   switchGpio.writeData0(0b00000000);
   switchGpio.setDirection1(0b00000000);
   switchGpio.writeData1(0b00000000);
}


void Switches::runTests() {
   switchGpio.writeData0(0b00000000);
   data = ~data;
   switchGpio.setDirection1(data);
}


void Switches::updateSwitches() {
   static uint8_t  lastButtonValue      = 0;
   static uint8_t  currentOutputValues  = 0;
   static uint8_t  latchedOutputValues  = 0;
   static unsigned stableCount          = 0;

   if (!This->power.isPowerOn()) {
      // Don't access peripherals is no power!
      return;
   }

   // Make Button GPIOs inputs
   This->switchGpio.setDirection1(0b00000000);

   // Poll switches - active low
   uint8_t  currentButtonValue   = 0;
   This->switchGpio.readData1(currentButtonValue);
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
         latchedOutputValues ^= currentButtonValue;
      }
   }
   // Restore LEDs (either 3-state or low)
   This->switchGpio.setDirection1(currentOutputValues|latchedOutputValues);
   // Update switch outputs
   This->switchGpio.writeData0(currentOutputValues|latchedOutputValues);
}
