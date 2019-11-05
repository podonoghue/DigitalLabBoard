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

/*
 * MOTOR SIMULATOR
 */
/// Motor simulator - Charlieplexed LEDs
using CharliePlexing = USBDM::GpioCField<4,0,USBDM::ActiveHigh>;
/// Motor simulator - Phase inputs from user
using MotorPhases    = USBDM::GpioCField<7,4,USBDM::ActiveHigh>;

/*
 * TRAFFIC SIMULATOR
 */
/// Traffic intersection - north-south car
using CarNorthSouthButton = USBDM::GpioD<0,  USBDM::ActiveLow>;
/// Traffic intersection - north-south pedestrian
using PedNorthSouthButton = USBDM::GpioD<0,  USBDM::ActiveLow>;
/// Traffic intersection - east-west car
using CarEastWestButton   = USBDM::GpioD<0,  USBDM::ActiveLow>;
/// Traffic intersection - east-west pedestrian
using PedEastWestButton   = USBDM::GpioD<0,  USBDM::ActiveLow>;

/// I2C address for Traffic intersection GPIO expander
/// IO0_7-0 = Fixed direction I/O to user
/// IO1_7-0 = Outputs to LEDs (active high)
static constexpr uint8_t TRAFFIC_I2C_ADDRESS = 0b001000010;

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
/// Frequency generator - Up button
using FrequencyUpButton   = USBDM::GpioB<17, USBDM::ActiveLow>;
/// Frequency generator - Down button
using FrequencyDownButton = USBDM::GpioB<16, USBDM::ActiveLow>;
/// Frequency generator - Timer being used
using FrequencyGeneratorTimer = USBDM::Ftm0;
/// Frequency generator - Timer channel being used
using FrequencyGeneratorTimerChannel = FrequencyGeneratorTimer::Channel<6>;
/// I2C Interface for OLED
using Oled_I2c = USBDM::I2c0;

/*
 * POWER CONTROL
 */
/// Power - Enable to voltage regulator
using PowerEnableControl  = USBDM::GpioB<3,  USBDM::ActiveHigh>;
/// Power - Enable button
using TestButton          = USBDM::GpioB<2,  USBDM::ActiveLow>;

/*
 * BUTTONS
 */
/// I2C address for debounced buttons GPIO expander
/// IO0_7-0 = Outputs to user
/// IO1_7-0 = Inputs from switches/Outputs to LEDs (active-low)
static constexpr uint8_t BUTTON_I2C_ADDRESS  = 0b001000000;

/*
 * SPARE PINS
 */
using Spare1  = USBDM::GpioA<4, USBDM::ActiveHigh>;
using Spare2  = USBDM::GpioD<4, USBDM::ActiveHigh>;
using Spare3  = USBDM::GpioD<5, USBDM::ActiveHigh>;

#endif /* SOURCES_CONFIGURE_H_ */
