/**
 * @file      hardware.h (180.ARM_Peripherals/Project_Headers/hardware.h)
 *
 * Main header file for USBDM library.
 * Generated code is included via this file.
 *
 * @version  V4.12.1.270
 * @date     1 December 2021
 */

/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */

#ifndef INCLUDE_USBDM_HARDWARE_H_
#define INCLUDE_USBDM_HARDWARE_H_

#include "error.h"
#include "pin_mapping.h"
#include "delay.h"
#include "console.h"

#include "gpio.h"
#include "pit.h"
#include "i2c.h"
#include "adc.h"
#include "ftm.h"


namespace USBDM {

/// ADC, Analogue Input
typedef const Adc0                                           MyAdc;                         

/// Used to measure JTAG Vref
typedef const Adc0::Channel<0>                               JtagVref;                      // ADC0_DP0(p7)

/// Used to measure Target Vdd
typedef const Adc0::Channel<7>                               TargetVddSample;               // PTD6(p47)

/// Frequency generator
typedef const Ftm0                                           FrequencyGeneratorTimer;       

/// Frequency generator channel
typedef const Ftm0::Channel<5>                               FrequencyGeneratorTimerChannel; // PTD5(p46)

/// Power toggle button
typedef const GpioA<4, ActiveLow>                            PowerButton;                   // PTA4(p21)

/// Down buttons
typedef const GpioB<17, ActiveLow>                           FrequencyDownButton;           // PTB17(p32)

/// Frequency generator - Up (Bit Field)
typedef const GpioBField<17, 16, ActiveLow>                  FrequencyButtons;              // PTB16(p31), PTB17(p32)

/// Down buttons
typedef const GpioB<16, ActiveLow>                           FrequencyUpButton;             // PTB16(p31)

/// Debug pin
typedef const GpioB<2>                                       Debug;                         // PTB2(p29)

/// Power - Enable to voltage regulator
typedef const GpioB<3>                                       PowerEnableControl;            // PTB3(p30)

/// Motor simulator - Charlieplexed LEDs (Bit Field)
typedef const GpioCField<3, 0>                               CharliePlexing;                // PTC0(p33), PTC1(p34), PTC2(p35), PTC3(p36)

/// Motor simulator - Phase inputs from user (Bit Field)
typedef const GpioCField<7, 4>                               MotorPhases;                   // PTC4(p37), PTC5(p38), PTC6(p39), PTC7(p40)

/// JTAG interface (Bit Field)
typedef const GpioDField<3, 0>                               Jtag;                          // PTD0(p41), PTD1(p42), PTD2(p43), PTD3(p44)

/// CAR_EW
typedef const GpioD<1>                                       CarEW;                         // PTD1(p42)

/// PED_NS
typedef const GpioD<2>                                       PedNS;                         // PTD2(p43)

/// Used to discharge TargetVdd (against leakage)
typedef const GpioD<6>                                       TargetVddDischarge;            // PTD6(p47)

/// Traffic Intersection (Bit Field)
typedef const GpioDField<3, 0>                               TrafficButtons;                // PTD0(p41), PTD1(p42), PTD2(p43), PTD3(p44)

/// JTAG - TDI
typedef const GpioD<2>                                       Tdi;                           // PTD2(p43)

/// PED_EW
typedef const GpioD<3>                                       PedEW;                         // PTD3(p44)

/// CAR_NS
typedef const GpioD<0>                                       CarNS;                         // PTD0(p41)

/// JTAG - TMS
typedef const GpioD<3>                                       Tms;                           // PTD3(p44)

/// Programmer Fail LED (Red)
typedef const GpioD<7>                                       ProgrammerFailLed;             // PTD7(p48)

/// JTAG - TCK
typedef const GpioD<0>                                       Tck;                           // PTD0(p41)

/// JTAG - TDO
typedef const GpioD<1>                                       Tdo;                           // PTD1(p42)

/// Programmer Pass LED (Green)
typedef const GpioD<4>                                       ProgrammerPassLed;             // PTD4(p45)

/// I2C Interface for OLED, and GPIOs
typedef I2c0                                                 I2cInterface;                  

/// Motor simulator - PIT channel
typedef const Pit::Channel<0>                                MotorPitChannel;               // PIT_CH0(Internal)

/// Used to poll directly connected buttons - power, clock and traffic
typedef const Pit::Channel<1>                                ButtonPollChannel;             // PIT_CH1(Internal)

} // End namespace USBDM

#endif /* INCLUDE_USBDM_HARDWARE_H_ */
