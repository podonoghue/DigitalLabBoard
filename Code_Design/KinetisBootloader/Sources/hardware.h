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

// No header file inclusions found

namespace USBDM {

/**
 * Map all configured pins to peripheral signals.
 *
 * PCRs of allocated pins are set according to settings in Configure.usbdmProject
 *
 * @note Only the lower 16-bits of the PCR registers are initialised
 */
extern void mapAllPins();
// /HARDWARE_H/Declarations not found
} // End namespace USBDM

#endif /* INCLUDE_USBDM_HARDWARE_H_ */
