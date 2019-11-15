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
#include "pit.h"

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
using MotorPitChannel = USBDM::Pit::Channel<0>;

/// Interval x5ms for switch debouncing
static constexpr unsigned DEBOUNCE_INTERVAL_COUNT = 20/5;

/// Interval x5ms for switch latching/unlatching
static constexpr unsigned LATCHED_INTERVAL_COUNT = 2000/5;

/// Interval x5ms for power switch delay
static constexpr unsigned POWER_BUTTON_DELAY_COUNT = 500/5;

/// Interval x5ms for power-up delay before notifying peripherals
/// This also affect the delay until power checks start (ADC)
static constexpr unsigned POWER_ON_DELAY_COUNT = 4;

/// Timer used for polling
using ButtonPollTimer = USBDM::Pit;
/// Used to poll directly connected buttons - power, clock and traffic
using ButtonPollChannel = USBDM::Pit::Channel<1>;

/*
 * TRAFFIC SIMULATOR
 */
/// Traffic intersection - buttons PED + CAR
using TrafficButtons = USBDM::GpioDField<3,0,USBDM::ActiveLow>;

/// I2C address for Traffic intersection GPIO expander
/// IO0_7-0 = Fixed direction I/O to user
/// IO1_7-0 = Outputs to LEDs (active high)
static constexpr uint8_t TRAFFIC_I2C_ADDRESS = 0b001;

/// Traffic intersection - Mask for east-west RED LED (GPIO expander outputs IO1_0)
static constexpr uint8_t EW_RED_LED_MASK          = 1U<<0;
/// Traffic intersection - Mask for east-west AMBER LED (GPIO expander outputs IO1_1)
static constexpr uint8_t EW_AMBER_LED_MASK        = 1U<<1;
/// Traffic intersection - Mask for east-west GREEN LED (GPIO expander outputs IO1_2)
static constexpr uint8_t EW_GREEN_LED_MASK        = 1U<<2;
/// Traffic intersection - Mask for east-west PEDESTRIAN GREEN LED (GPIO expander outputs IO1_3)
static constexpr uint8_t EW_PEDESTRIAN_LED_MASK   = 1U<<3;
/// Traffic intersection - Mask for north-south RED LED (GPIO expander outputs IO1_4)
static constexpr uint8_t NS_RED_LED_MASK          = 1U<<4;
/// Traffic intersection - Mask for north-south AMBER LED (GPIO expander outputs IO1_5)
static constexpr uint8_t NS_AMBER_LED_MASK        = 1U<<5;
/// Traffic intersection - Mask for north-south GREEN LED (GPIO expander outputs IO1_6)
static constexpr uint8_t NS_GREEN_LED_MASK        = 1U<<6;
/// Traffic intersection - Mask for north-south PEDESTRIAN GREEN LED (GPIO expander outputs IO1_7)
static constexpr uint8_t NS_PEDESTRIAN_LED_MASK   = 1U<<7;

/// Traffic intersection - Mask for east-west PEDESTRIAN BUTTON output to user (GPIO expander outputs IO0_7)
static constexpr uint8_t EW_PED_OUT_MASK          = 1U<<7;
/// Traffic intersection - Mask for north-south PEDESTRIAN BUTTON output to user (GPIO expander outputs IO0_6)
static constexpr uint8_t NS_PED_OUT_MASK          = 1U<<6;
/// Traffic intersection - Mask for east-west CAR BUTTON output to user (GPIO expander outputs IO0_5)
static constexpr uint8_t EW_CAR_OUT_MASK          = 1U<<5;
/// Traffic intersection - Mask for north-south CAR BUTTON output to user (GPIO expander outputs IO0_4)
static constexpr uint8_t NS_CAR_OUT_MASK          = 1U<<5;

/// Traffic intersection - Mask for east-west LIGHTS input from user (GPIO expander outputs IO0_3-2)
static constexpr uint8_t EW_LIGHTS_IN_MASK        = 3U<<2;
/// Traffic intersection - Mask for north-south LIGHTS input from user (GPIO expander outputs IO0_1-0)
static constexpr uint8_t NS_LIGHTS_IN_MASK        = 3U<<0;

/*
 * FREQUENCY GENERATOR
 */
/// Frequency generator - Up/Down buttons
using FrequencyButtons   = USBDM::GpioBField<17,16, USBDM::ActiveLow>;
/// Frequency generator - Timer being used
using FrequencyGeneratorTimer = USBDM::Ftm0;
/// Frequency generator - Timer channel being used
using FrequencyGeneratorTimerChannel = FrequencyGeneratorTimer::Channel<6>;

/*
 * POWER CONTROL
 */
/// Power - Enable to voltage regulator
using PowerEnableControl  = USBDM::GpioB<3,  USBDM::ActiveHigh>;
/// Power - Enable button
using PowerButton          = USBDM::GpioA<4,  USBDM::ActiveLow>;
// Used to sample the target power supply
using VddSample = USBDM::Adc0::Channel<0>;
/*
 * BUTTONS
 */
/// I2C address for debounced buttons GPIO expander
/// IO0_7-0 = Outputs to user
/// IO1_7-0 = Inputs from switches/Outputs to LEDs (active-low)
static constexpr uint8_t BUTTON_I2C_ADDRESS  = 0b000;

/*
 * SPARE PINS
 */
using Spare1  = USBDM::GpioA<4, USBDM::ActiveHigh>;
using Spare2  = USBDM::GpioD<4, USBDM::ActiveHigh>;
using Spare3  = USBDM::GpioD<5, USBDM::ActiveHigh>;

#endif /* SOURCES_CONFIGURE_H_ */
