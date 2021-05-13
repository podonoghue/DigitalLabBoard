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

using namespace USBDM;

/**
 * Used to initialise shared hardware before static declarations
 */
class Initialise {

   static void buttonPollTimerCallback();
   PitChannelNum channel = PitChannelNum_None;

public:

   /**
    * Initialise:
    * - Adc
    * - Pit
    * - Button polling using a PIT channel (initially disabled)
    */
   Initialise() {
      // Shared PIT
      USBDM::Pit::configure();
      channel = Pit::allocateChannel();
      Pit::setCallback(channel, buttonPollTimerCallback);
      Pit::configureChannel(channel, 5*ms, PitChannelIrq_Enabled);

      // Shared ADC
      Adc0::configure(AdcResolution_8bit_se, AdcClockSource_Busdiv2);
      Adc0::setAveraging(AdcAveraging_4);
   }

   void enablePolling() {
      Pit::enableNvicInterrupts(channel, NvicPriority_Normal);
   }

   void disablePolling() {
      Pit::enableNvicInterrupts(channel, NvicPriority_Normal);
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

struct BootInformation {
   uint32_t reserved;
   uint32_t softwareVersion;
   uint32_t hardwareVersion;
   uint32_t checksum;
};

static constexpr unsigned HARDWARE_VERSION = HW_LOGIC_BOARD_V4;

__attribute__ ((section(".bootloader")))
__attribute__((used))
static BootInformation const bootInformation = {
      0,                   // Reserved
      3,                   // Software version
      HARDWARE_VERSION,    // Hardware version for this image
      0,                   // Checksum - filled in by loader
};

int main() {
   // Enable Reset pin filter
   Rcm::configure(
         RcmResetPinRunWaitFilter_LowPowerOscillator,
         RcmResetPinStopFilter_LowPowerOscillator,
         24);

   // Set first power-on message
   StringFormatter_T<22> sf;
   sf.write("SW:V").writeln(bootInformation.softwareVersion)
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
