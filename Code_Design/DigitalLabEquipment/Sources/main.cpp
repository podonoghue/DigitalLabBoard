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
#include "stringFormatter.h"
#include "FunctionQueue.h"
#include "PCA9555.h"
#include "FrequencyGenerator.h"
#include "MotorSimulation.h"
#include "Switches.h"
#include "Power.h"
#include "Traffic.h"
#include "pit.h"
#include "rcm.h"
#include "UsbCommandMessage.h"
#include "xsf_usb_player.h"
#include "BootInformation.h"

using namespace USBDM;

/**
 * Used to initialise shared hardware before static declarations
 */
class Initialise {

   static void buttonPollTimerCallback();

public:

   /**
    * Initialise:
    * - Adc
    * - Pit
    * - Button polling using a PIT channel (initially disabled)
    */
   Initialise() {
      // Shared PIT
      Pit::configure();
      ButtonPollChannel::setCallback(buttonPollTimerCallback);
      ButtonPollChannel::configure(5_ms, PitChannelIrq_Enabled);

      // Shared ADC
      Adc0::configure(AdcResolution_8bit_se, AdcClockSource_Busdiv2);
      Adc0::setAveraging(AdcAveraging_4);
   }

   void enablePolling() {
      ButtonPollChannel::enableNvicInterrupts(NvicPriority_Normal);
   }

   void disablePolling() {
      ButtonPollChannel::enableNvicInterrupts(NvicPriority_Normal);
   }
};

// Must be first object created
static Initialise           initialise;

static Power                powerControl{};
static I2cInterface         i2c{};
static Oled                 oled(i2c);
static FunctionQueue        functionQueue{};
static MotorSimulator       motorSimulator(functionQueue, powerControl);
static FrequencyGenerator   frequencyGenerator(oled, functionQueue, powerControl);
static Switches             switches(i2c, functionQueue, powerControl);
static Traffic              traffic(i2c, functionQueue, powerControl);
static UsbXsvfInterface     usbXsvfInterface(powerControl);

/**
 * Timer call-back to poll buttons @ 5ms interval
 */
void Initialise::buttonPollTimerCallback() {
   powerControl.pollPower();
   frequencyGenerator.pollButtons();
   switches.pollButtons();
   traffic.pollButtons();
}

static constexpr unsigned HARDWARE_VERSION = HW_LOGIC_BOARD_V4a;

__attribute__ ((section(".noinit")))
static uint32_t magicNumber;

#if defined(RELEASE_BUILD)
// Triggers memory image relocation for bootloader
extern BootInformation const bootloaderInformation;
#endif

__attribute__ ((section(".bootloaderInformation")))
__attribute__((used))
const BootInformation bootloaderInformation = {
      &magicNumber,        // Magic number to force ICP on reboot
      4,                   // Version of this software image
      HARDWARE_VERSION,    // Hardware version for this image
};

/**
 * Reset system to bootloader mode
 */
__attribute__((unused))
static void resetToBootloader() {

#ifndef SCB_AIRCR_VECTKEY
#define SCB_AIRCR_VECTKEY(x) (((x)<<SCB_AIRCR_VECTKEY_Pos)&SCB_AIRCR_VECTKEY_Msk)
#endif

   // Set ICP on reboot
   magicNumber = MAGIC_NUMBER;

   // Request system reset
   SCB->AIRCR = SCB_AIRCR_VECTKEY(0x5FA) | SCB_AIRCR_SYSRESETREQ_Msk;

   // Wait until reset
   for(;;) {
      __asm__("nop");
   }
}

int main() {
//   console.setBaudRate(defaultBaudRate);
//   for(;;) {
//      console.writeln("Starting\n");
//      wait(100_ms);
//   }
   // Enable Reset pin filter
   Rcm::configure(
         RcmResetPinRunWaitFilter_LowPowerOscillator,
         RcmResetPinStopFilter_LowPowerOscillator,
         RcmResetFilterBusClockCount_32);

   // Set message for first power-on
   StringFormatter_T<22> sf;
   sf.write("SW:V").writeln(bootloaderInformation.softwareVersion)
     .write("HW:").write(getHardwareVersion<HARDWARE_VERSION>());
   frequencyGenerator.setStartupMessage(sf.toString());

   for(;;) {
      // Poll USB
      initialise.disablePolling();
      usbXsvfInterface.pollUsb();
      initialise.enablePolling();

      // Execute functions from the function queue
      if (!functionQueue.isEmpty()) {
         auto f = functionQueue.deQueue();
         if (f != nullptr) {
            f();
         }
      }
   }
   return 0;
}
