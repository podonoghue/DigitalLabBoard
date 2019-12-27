/*
 ============================================================================
 * @file    main.cpp
 * @brief   Main-line for Digital Logic Board
 *
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include "Configure.h"
#include "FunctionQueue.h"
#include "PCA9555.h"
#include "FrequencyGenerator.h"
#include "MotorSimulation.h"
#include "Switches.h"
#include "Power.h"
#include "Traffic.h"
#include "pit.h"

using namespace USBDM;

/**
 * Used to initialise some hardware before static declarations
 */
class Initialise {

   static void buttonPollTimerCallback();

   PitChannelNum channel = PitChannelNum_None;

public:

   /**
    * Initialise:
    * - Adc
    * - Pit
    * - Button polling using a PIT channel (initially disabled)
    */
   Initialise() {
      // Shared PIT
      USBDM::Pit::configure();
      channel = Pit::allocateChannel();
      Pit::setCallback(channel, buttonPollTimerCallback);
      Pit::configureChannel(channel, 5*ms, PitChannelIrq_Enabled);

      // Shared ADC
      Adc0::configure(AdcResolution_8bit_se, AdcClockSource_Busdiv2);
      Adc0::setAveraging(AdcAveraging_4);
   }

   void enablePolling() {
      Pit::enableNvicInterrupts(channel, NvicPriority_Normal);
   }

   void disablePolling() {
      Pit::enableNvicInterrupts(channel, NvicPriority_Normal);
   }
};

static Initialise           initialise;

static Power                powerControl{};
static I2cInterface         i2c{};
static Oled                 oled(i2c);
static FunctionQueue        functionQueue{};
static MotorSimulator       motorSimulator(functionQueue, powerControl);
static FrequencyGenerator   frequencyGenerator(oled, functionQueue, powerControl);
static Switches             switches(i2c, functionQueue, powerControl);
static Traffic              traffic(i2c, functionQueue, powerControl);

/**
 * Timer call-back to poll buttons @ 5ms interval
 */
void Initialise::buttonPollTimerCallback() {
   powerControl.pollPower();
   frequencyGenerator.pollButtons();
   switches.pollButtons();
   traffic.pollButtons();
}

extern void pollUsb();

int main() {

   // Execute functions from the function queue
   for(;;) {
      initialise.disablePolling();
      pollUsb();
      initialise.enablePolling();
      if (!functionQueue.isEmpty()) {
         auto f = functionQueue.deQueue();
         if (f != nullptr) {
            f();
         }
      }
   }
   return 0;
}
