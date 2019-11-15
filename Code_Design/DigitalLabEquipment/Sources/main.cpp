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

using namespace USBDM;

Power                powerControl{};
I2cInterface         i2c{};
Oled                 oled(i2c);
FunctionQueue        functionQueue{};
MotorSimulator       motorSimulator{};
FrequencyGenerator   frequencyGenerator(oled, functionQueue, powerControl);
Switches             switches(i2c, functionQueue, powerControl);
Traffic              traffic(i2c, functionQueue, powerControl);

/**
 * Timer call-back to poll buttons @ 5ms interval
 */
void buttonPollTimerCallback() {
   powerControl.pollPower();
   if (powerControl.isPowerOn()) {
      frequencyGenerator.pollButtons();
      switches.pollButtons();
      traffic.pollButtons();
   }
}

/**
 * Initialise the button polling using a PIT channel
 */
void configureButtonPolling() {
   ButtonPollTimer::configure(PitDebugMode_Stop);
   ButtonPollChannel::setCallback(buttonPollTimerCallback);
   ButtonPollChannel::enableNvicInterrupts(NvicPriority_Low);
   ButtonPollChannel::configure(5*ms, PitChannelIrq_Enabled);
}

int main() {

   configureButtonPolling();

   // Execute functions from the function queue
   for(;;) {
      if (!functionQueue.isEmpty()) {
         auto f = functionQueue.deQueue();
         if (f != nullptr) {
            f();
         }
      }
   }
   return 0;
}
