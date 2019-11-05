/**
 =================================================================================
 * @file    Waveform.cpp
 * @brief   Generate square waves over a wide range of frequencies using the FTM
 =================================================================================
 */
#include <FrequencyGenerator.h>
#include "hardware.h"
#include "Configure.h"

using namespace USBDM;

// LED for debug checks
using Led = Spare1;

/**
 * Interrupt handler for FrequencyGeneratorTimer interrupts
 * This sets the next interrupt/pin toggle for a half-period from the last event
 *
 * @param[in] status Flags indicating interrupt source channel(s)
 */
static void ftmCallback(uint8_t status) {
   static unsigned iterationCounter = 0;

   // Check channel
   if (status & FrequencyGeneratorTimerChannel::CHANNEL_MASK) {
      iterationCounter++;
      if (iterationCounter >= 100) {
         iterationCounter = 0;
         // Set on next edge
         FrequencyGeneratorTimerChannel::setMode(FtmChMode_OutputCompareSet);
      }
      else if (iterationCounter == 50) {
         // Clear on next edge
         FrequencyGeneratorTimerChannel::setMode(FtmChMode_OutputCompareClear);
      }
      // Note: The pin is toggled directly by hardware
      // Counter roll-over is used to control interval
   }
}

void FrequencyGenerator::initialiseWaveform() {
   Led::setOutput();

   /**
    * FTM channel set as Output compare with pin Toggle mode and using a callback function
    */
   // Configure base FTM (affects all channels)
   FrequencyGeneratorTimer::configure(
         FtmMode_LeftAlign,       // Left-aligned is required for OC/IC
         FtmClockSource_System);  // Bus clock most accurate source

   // Set callback function (may be shared by multiple channels)
   FrequencyGeneratorTimer::setChannelCallback(ftmCallback);

   // Enable FTM interrupts (required for channel interrupts)
   FrequencyGeneratorTimer::enableNvicInterrupts(NvicPriority_MidHigh);

   // Configure pin associated with channel
   FrequencyGeneratorTimerChannel::setOutput(
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinSlewRate_Slow);

   // Check if configuration failed
   USBDM::checkError();
}

/**
 * Set frequency of generator
 *
 * @param frequency
 *
 * @note Frequency generator is initialised on 1st call to this routine.
 */
void FrequencyGenerator::setFrequency(unsigned frequency) {
   usbdm_assert((frequency == Frequency_Off)||((frequency>=Frequency_Min)&&(frequency<=Frequency_Max)), "Illegal frequency");

   currentFrequency = frequency;
   if (frequency == Frequency_Off) {
      /*
       * Off strategy
       *
       * No interrupts
       * Output pin stays low (cleared on all events)
       */
      FrequencyGeneratorTimerChannel::configure(FtmChMode_OutputCompareClear, FtmChannelAction_None);
   }
   else if (frequency < 100*Hz) {
      /*
       * Low frequency strategy
       *
       * Interrupts
       * Event time is manipulated by call=back.
       * Output pin action manipulated by call-back
       * Set x50, Clear x50 so effectively /100 of event frequency
       */
      FrequencyGeneratorTimer::setPeriod(1/(100.0*frequency));
      FrequencyGeneratorTimerChannel::configure(FtmChMode_OutputCompareClear, FtmChannelAction_Irq);
   }
   else {
      /*
       * High frequency strategy
       * This method is not commonly used as it monopolised the FTM i.e. it makes it difficult to
       * use different channels for independent uses.
       *
       * No interrupts
       * Output pin action is toggle on all events
       * Event at fixed counter value (1 tick)
       * Event interval is controlled by changing the FTM period.
       */
      FrequencyGeneratorTimerChannel::configure(FtmChMode_OutputCompareToggle, FtmChannelAction_None);

      FrequencyGeneratorTimer::setPeriod(1/(2.0*frequency));

      // Trigger pin action when counter crosses 1 tick
      FrequencyGeneratorTimerChannel::setEventTime(1);
   }

   displayFrequency(currentFrequency);
}

/**
 * Get the current waveform frequency
 *
 * @return
 */
unsigned FrequencyGenerator::getFrequency() {
   return currentFrequency;
}

/**
 * Display Frequency on LCD
 *
 * @param frequency
 */
void FrequencyGenerator::displayFrequency(unsigned frequency) {
   const char *units = "    ";

   oled.clearDisplay();
   oled.setFont(fontVeryLarge);

   if (frequency == Frequency_Off) {
      oled.write("  Off");
   }
   else {
      if (frequency >= 1*MHz) {
         units = " MHz";
         frequency /= 1000000;
      }
      else if (frequency >= 1*kHz) {
         units = " kHz";
         frequency /= 1000;
      }
      else if (frequency != 0) {
         units = "  Hz";
      }
      oled.setWidth(4).setPadding(Padding_LeadingSpaces).write(frequency).write(units).write(" ");
   }
   oled.refreshImage();
   oled.resetFormat();
}

/**
 * Test loop for debug
 */
void FrequencyGenerator::testLoop() {
   float freqs[] = {
         0,
         1,2,5,
         10,20,50,
         100,200,500,
         1*kHz,2*kHz,5*kHz,
         10*kHz,20*kHz,50*kHz,
         100*kHz,200*kHz,500*kHz,
         1*MHz,2*MHz,5*MHz,
   };
   for(;;) {
      for (unsigned index=0; index<sizeof(freqs)/sizeof(freqs[0]); index++) {
         setFrequency(freqs[index]);
         waitMS(2000);
      }
   }

}
