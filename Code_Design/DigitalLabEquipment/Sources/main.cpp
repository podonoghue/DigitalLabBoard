/*
 ============================================================================
 * @file    main.cpp (180.ARM_Peripherals/Sources/main.cpp)
 * @brief   Basic C++ demo
 *
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include <FrequencyGenerator.h>
#include "hardware.h"
#include "Configure.h"
#include "MotorSimulation.h"
#include "SwitchDebouncer.h"
#include "Oled.h"

using namespace USBDM;

Oled_I2c             i2c{};
Oled                 oled(i2c);
FrequencyGenerator   frequencyGenerator(oled);

int main() {

   initialiseMotorSimulation();
   initialiseSwitchDebouncer();

   frequencyGenerator.testLoop();

   for(;;) {
      __asm__("nop");
   }
   return 0;
}
