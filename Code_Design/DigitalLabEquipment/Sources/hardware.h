/**
 * @file      hardware.h (180.ARM_Peripherals/Sources/hardware.h)
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

/**
 * Map all configured pins to peripheral signals.
 *
 * PCRs of allocated pins are set according to settings in Configure.usbdmProject
 *
 * @note Only the lower 16-bits of the PCR registers are initialised
 */
extern void mapAllPins();

/// ADC, Analogue Input
typedef const Adc0                                           MyAdc;                                        

/// Used to measure JTAG Vref
typedef const Adc0::Channel<0>                               JtagVref;                                     // ADC0_DP0(p7)

/// Used to measure Target Vdd
typedef const Adc0::Channel<7>                               TargetVddSample;                              // PTD6(p47)

/// Frequency generator
typedef const Ftm0                                           FrequencyGeneratorTimer;                      

/// Frequency generator channel
typedef const Ftm0::Channel<5>                               FrequencyGeneratorTimerChannel;               // PTD5(p46)

/// Power toggle button
typedef const GpioTable_T<GpioAInfo, 4, ActiveLow>           PowerButton;                                  // PTA4(p21)

/// Down buttons
typedef const GpioTable_T<GpioBInfo, 16, ActiveLow>          FrequencyDownButton;                          // PTB16(p31)

/// Frequency generator - Up (Bit Field)
typedef const GpioFieldTable_T<GpioBInfo, 17, 16, ActiveLow> FrequencyButtons;                             // PTB16(p31), PTB17(p32)

/// Down buttons
typedef const GpioTable_T<GpioBInfo, 17, ActiveLow>          FrequencyUpButton;                            // PTB17(p32)

/// Debug pin
typedef const GpioTable_T<GpioBInfo, 2, ActiveHigh>          Debug;                                        // PTB2(p29)

/// Power - Enable to voltage regulator
typedef const GpioTable_T<GpioBInfo, 3, ActiveHigh>          PowerEnableControl;                           // PTB3(p30)

/// Motor simulator - Charlieplexed LEDs (Bit Field)
typedef const GpioFieldTable_T<GpioCInfo, 3, 0, ActiveHigh>  CharliePlexing;                               // PTC0(p33), PTC1(p34), PTC2(p35), PTC3(p36)

/// Motor simulator - Phase inputs from user (Bit Field)
typedef const GpioFieldTable_T<GpioCInfo, 7, 4, ActiveHigh>  MotorPhases;                                  // PTC4(p37), PTC5(p38), PTC6(p39), PTC7(p40)

/// JTAG interface (Bit Field)
typedef const GpioFieldTable_T<GpioDInfo, 3, 0, ActiveLow>   Jtag;                                         // PTD0(p41), PTD1(p42), PTD2(p43), PTD3(p44)

/// CAR_EW
typedef const GpioTable_T<GpioDInfo, 1, ActiveLow>           CarEW;                                        // PTD1(p42)

/// PED_NS
typedef const GpioTable_T<GpioDInfo, 2, ActiveLow>           PedNS;                                        // PTD2(p43)

/// Used to discharge TargetVdd (against leakage)
typedef const GpioTable_T<GpioDInfo, 6, ActiveHigh>          TargetVddDischarge;                           // PTD6(p47)

/// Traffic Intersection (Bit Field)
typedef const GpioFieldTable_T<GpioDInfo, 3, 0, ActiveLow>   TrafficButtons;                               // PTD0(p41), PTD1(p42), PTD2(p43), PTD3(p44)

/// JTAG - TDI
typedef const GpioTable_T<GpioDInfo, 2, ActiveLow>           Tdi;                                          // PTD2(p43)

/// PED_EW
typedef const GpioTable_T<GpioDInfo, 3, ActiveLow>           PedEW;                                        // PTD3(p44)

/// CAR_NS
typedef const GpioTable_T<GpioDInfo, 0, ActiveLow>           CarNS;                                        // PTD0(p41)

/// JTAG - TMS
typedef const GpioTable_T<GpioDInfo, 3, ActiveLow>           Tms;                                          // PTD3(p44)

/// Programmer Fail LED (Red)
typedef const GpioTable_T<GpioDInfo, 7, ActiveHigh>          ProgrammerFailLed;                            // PTD7(p48)

/// JTAG - TCK
typedef const GpioTable_T<GpioDInfo, 0, ActiveLow>           Tck;                                          // PTD0(p41)

/// JTAG - TDO
typedef const GpioTable_T<GpioDInfo, 1, ActiveLow>           Tdo;                                          // PTD1(p42)

/// Programmer Pass LED (Green)
typedef const GpioTable_T<GpioDInfo, 4, ActiveHigh>          ProgrammerPassLed;                            // PTD4(p45)

/// I2C Interface for OLED, and GPIOs
typedef I2c0                                                 I2cInterface;                                 

/// Motor simulator - PIT channel
typedef const Pit::Channel<0>                                MotorPitChannel;                              // PIT_CH0(Internal)

/// Used to poll directly connected buttons - power, clock and traffic
typedef const Pit::Channel<1>                                ButtonPollChannel;                            // PIT_CH1(Internal)

} // End namespace USBDM

#endif /* INCLUDE_USBDM_HARDWARE_H_ */