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
//#include "pit.h"

/// I2C Interface for OLED, and GPIOs
using I2cInterface = USBDM::I2c0;

/*
 * MOTOR SIMULATOR
 */
/// Motor simulator - Charlieplexed LEDs
using CharliePlexing = USBDM::GpioCField<4,0,USBDM::ActiveHigh>;
/// Motor simulator - Phase inputs from user
using MotorPhases    = USBDM::GpioCField<7,4,USBDM::ActiveHigh>;
/// Motor simulator - PIT channel
//using MotorPitChannel = USBDM::Pit::Channel<0>;

/// Interval x5ms for switch debouncing/unlatching
static constexpr unsigned DEBOUNCE_INTERVAL_COUNT = 20/5;

/// Interval x5ms for switch latching
static constexpr unsigned LATCHED_INTERVAL_COUNT = 2000/5;

/// Interval x5ms for power switch delay
static constexpr unsigned POWER_BUTTON_DELAY_COUNT = 500/5;

/// Interval x5ms for power-up delay before power checks start (ADC)
static constexpr unsigned POWER_ON_ADC_DELAY_COUNT = 20/5;

/// Interval x5ms for power-up delay before notifying peripherals
static constexpr unsigned POWER_ON_DELAY_COUNT = 100/5;

/// Timer used for polling
//using ButtonPollTimer = USBDM::Pit;

/// Used to poll directly connected buttons - power, clock and traffic
//using ButtonPollChannel = USBDM::Pit::Channel<1>;

/*
 * TRAFFIC SIMULATOR
 */
/// Traffic intersection - buttons PED + CAR
using TrafficButtons = USBDM::GpioDField<3,0,USBDM::ActiveLow>;

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
   constexpr unsigned NS_LED_OFFSET = 0;
   constexpr unsigned EW_LED_OFFSET = 4;

   return (lightTable[nsLights]<<NS_LED_OFFSET)|(lightTable[ewLights]<<EW_LED_OFFSET);
}

/*
 * FREQUENCY GENERATOR
 */
/// Frequency generator - Up/Down buttons
using FrequencyButtons   = USBDM::GpioBField<17,16, USBDM::ActiveLow>;
/// Frequency generator - Mask for UP button (see FrequencyButtons)
static constexpr unsigned FREQUENCY_UP_BUTTON = 0b10;
/// Frequency generator - Mask for DOWN button (see FrequencyButtons)
static constexpr unsigned FREQUENCY_DOWN_BUTTON = 0b01;
/// Frequency generator - Timer being used
using FrequencyGeneratorTimer = USBDM::Ftm0;
/// Frequency generator - Timer channel being used
using FrequencyGeneratorTimerChannel = FrequencyGeneratorTimer::Channel<5>;

/*
 * POWER CONTROL
 */
/// Power - Enable to voltage regulator
using PowerEnableControl  = USBDM::GpioB<3,  USBDM::ActiveHigh>;
/// Power - Enable button
using PowerButton          = USBDM::GpioA<4,  USBDM::ActiveLow>;
/// Power - Used to sample the target power supply (alias for TargetVddDischarge)
using TargetVddSample = USBDM::Adc0::Channel<7>;
/// Power - Used to discharge the target power supply (alias for TargetVddSample)
using TargetVddDischarge = USBDM::GpioD<6, USBDM::ActiveLow>;

/*
 * BUTTONS
 */
/// I2C address for debounced buttons GPIO expander
/// IO0_7-0 = Outputs to user
/// IO1_7-0 = Inputs from switches/Outputs to LEDs (active-low)
static constexpr uint8_t BUTTON_I2C_ADDRESS  = 0b000;

// Programmer LEDs
using ProgrammerOkLed   = USBDM::GpioD<4>;
using ProgrammerBusyLed = USBDM::GpioD<7>;


#endif /* SOURCES_CONFIGURE_H_ */
