/*
 ============================================================================
 * @file    main.cpp (180.ARM_Peripherals/Sources/main.cpp)
 * @brief   Basic C++ demo
 *
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include "hardware.h"
#include "Oled.h"
#include "Configure.h"
#include "FrequencyGenerator.h"
#include "MotorSimulation.h"
#include "Switches.h"
#include "Power.h"
#include "FunctionQueue.h"
#include "PCA9555.h"

using namespace USBDM;

I2cInterface         i2c{};
Oled                 oled(i2c);
FunctionQueue        functionQueue;
FrequencyGenerator   frequencyGenerator(oled, functionQueue);
Power                powerControl(frequencyGenerator);
Switches             switches(i2c, powerControl, functionQueue);

Pca9555              trafficGpio(i2c, TRAFFIC_I2C_ADDRESS);
MotorSimulator       motorSimulator;

class Traffic {
private:
   static Traffic *This;

   USBDM::Pca9555 trafficGpio;
   Power          &power;

   FunctionQueue  &functionQueue;

   static void updateSwitches() {
      static uint8_t  lastButtonValue      = 0;
      static unsigned stableCount          = 0;

      if (!This->power.isPowerOn()) {
         // Don't access peripherals is no power!
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
            This->trafficGpio.writeData0(currentButtonValue<<4);
         }
      }
      uint8_t lightInputs;
      This->trafficGpio.readData0(lightInputs);

      static const uint8_t lightTable[] = {
            0b0001,  // Red
            0b0010,  // Amber
            0b0100,  // Green
            0b1100,  // Green + Walk
      };
      uint8_t ledMask = 0;
      ledMask |= lightTable[lightInputs&0b11];
      ledMask |= lightTable[(lightInputs>>2)&0b11]<<4;

      // Update LEDs
      This->trafficGpio.writeData1(ledMask);
   }

public:
   Traffic(USBDM::I2c &i2c, Power &power, FunctionQueue &functionQueue) :
      trafficGpio(USBDM::Pca9555(i2c,  TRAFFIC_I2C_ADDRESS)), power(power), functionQueue(functionQueue) {
      usbdm_assert((This == nullptr), "Only single instance of Traffic allowed");
      This = this;

      TrafficButtons::setInput(PinPull_None, PinAction_None, PinFilter_None);

      trafficGpio.setDirection0(0b11110000);
      trafficGpio.setDirection1(0b11111111);
      trafficGpio.writeData0(0b0000);
      trafficGpio.writeData0(0b00000000);
   }

   void pollButtons() {
      functionQueue.enQueueDiscardOnFull(updateSwitches);
   }

};

Traffic *Traffic::This = nullptr;

Traffic traffic(i2c, powerControl, functionQueue);

/**
 * Timer call-back to poll buttons @ 5ms interval
 */
void buttonPollTimerCallback() {
   powerControl.pollPower();
   frequencyGenerator.pollButtons();
   switches.pollButtons();
   traffic.pollButtons();
}

void configureButtonPolling() {
//   ButtonPollTimer::configure(PitDebugMode_Stop);
   ButtonPollChannel::setCallback(buttonPollTimerCallback);
   ButtonPollChannel::enableNvicInterrupts(NvicPriority_Low);
   ButtonPollChannel::configure(5*ms, PitChannelIrq_Enabled);
}

int main() {

   configureButtonPolling();

   waitMS(1000);

   //   frequencyGenerator.testLoop();
   //   testMotorLeds();

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
