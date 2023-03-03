/*
 * Configure.h
 *
 *  This file defines how the microcontroller pins and peripherals are being used.
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_CONFIGURE_H_
#define SOURCES_CONFIGURE_H_

#include "hardware.h"
#include "i2c.h"

/// Interval x5ms for switch debouncing/unlatching
static constexpr unsigned DEBOUNCE_INTERVAL_COUNT = 20/5;

/// Interval x5ms for switch latching
static constexpr unsigned LATCHED_INTERVAL_COUNT = 2000/5;

/// Interval x5ms for switch initial auto-repeat for frequency
static constexpr unsigned AUTO_REPEAT_COUNT = 1000/5;

/// Interval x5ms for switch auto-repeat for frequency
static constexpr unsigned AUTO_REPEAT_RATE = 500/5;

/// Interval x5ms for power switch delay
static constexpr unsigned POWER_BUTTON_DELAY_COUNT = 200/5;

/// Interval x5ms for power-up delay before power checks start (ADC)
static constexpr unsigned POWER_ON_ADC_DELAY_COUNT = 20/5;

/// Interval x5ms for power-up delay before notifying peripherals
static constexpr unsigned POWER_ON_DELAY_COUNT = 100/5;

/*
 * TRAFFIC SIMULATOR
 */
/// I2C address for Traffic intersection GPIO expander
/// IO0_7-0 = Fixed direction user I/O
/// IO1_7-0 = Outputs to LEDs (active high)
static constexpr uint8_t TRAFFIC_I2C_ADDRESS = 0b001;

/**
 * Extract north-south lights from user input
 *
 * @param portValue Port value from IO0_7-0
 *
 * @return 0-3 indicating light encoding
 */
static constexpr inline unsigned northSouthLights(unsigned portValue) {
   // Traffic intersection - Mask for north-south LIGHTS input from user (GPIO expander outputs IO0_1-0)
   constexpr uint8_t NS_LIGHTS_IN_OFFSET      = 0;
   constexpr uint8_t NS_LIGHTS_IN_MASK        = 0b11U<<NS_LIGHTS_IN_OFFSET;

   return (portValue & NS_LIGHTS_IN_MASK)>>NS_LIGHTS_IN_OFFSET;
}

/**
 * Extract east-west lights from user input
 *
 * @param portValue Port value from IO0_7-0
 *
 * @return 0-3 indicating light encoding
 */
static constexpr inline unsigned eastWestLights(unsigned portValue) {
   // Traffic intersection - Masks for LIGHTS input from user (GPIO expander outputs IO0_3-2)
   constexpr uint8_t EW_LIGHTS_IN_OFFSET      = 2;
   constexpr uint8_t EW_LIGHTS_IN_MASK        = 0b11U<<(EW_LIGHTS_IN_OFFSET);

   return (portValue & EW_LIGHTS_IN_MASK)>>EW_LIGHTS_IN_OFFSET;
}

/**
 * Encode north-south and east-west light values to LED mask
 *
 * @param nsLights   North-south lights input
 * @param ewLights   East-west lights input
 */
static constexpr inline unsigned trafficLedsEncode(unsigned nsLights, unsigned ewLights) {
   /// Traffic intersection - Mask for LEDs (GPIO expander outputs IO1_(3..0)/IO1_(7..4))
   enum {
      RED_LED_MASK         = 0b0001,
      AMBER_LED_MASK       = 0b0010,
      GREEN_LED_MASK       = 0b0100,
      PEDESTRIAN_LED_MASK  = 0b1000,
   };

   // Encoding for traffic LEDs
   const uint8_t lightTable[] = {
         RED_LED_MASK,                          // Red
         AMBER_LED_MASK,                        // Amber
         GREEN_LED_MASK,                        // Green
         GREEN_LED_MASK|PEDESTRIAN_LED_MASK,    // Green + Walk
   };

   // Offsets for LEDs (GPIO expander outputs IO1_(7..4)/IO1_(3..0))
   constexpr unsigned EW_LED_OFFSET = 0;
   constexpr unsigned NS_LED_OFFSET = 4;

   return (lightTable[nsLights]<<NS_LED_OFFSET)|(lightTable[ewLights]<<EW_LED_OFFSET);
}

/// I2C address for debounced buttons GPIO expander
/// IO0_7-0 = Outputs to user
/// IO1_7-0 = Inputs from switches/Outputs to LEDs (active-low)
static constexpr uint8_t BUTTON_I2C_ADDRESS  = 0b000;

#endif /* SOURCES_CONFIGURE_H_ */
