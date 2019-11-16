/*
 * MotorSImulation.cpp
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */

#include "MotorSimulation.h"
#include "Configure.h"
#include "pit.h"

using namespace USBDM;

/**
 * Data for 1 Charlieplexed LED
 */
struct CharlieData {
   uint8_t highPin;
   uint8_t lowPin;
};

/**
 * Data for Charlieplexed LEDs
 * In clockwise sequence
 */
static const CharlieData charlieData[] = {
      // High    Low
      // 3210    3210
      {0b0000, 0b0000}, // Off

      {0b0001, 0b1000}, // Physical LED #12
      {0b1000, 0b0001}, // Physical LED #11
      {0b0010, 0b1000}, // Physical LED #10
      {0b1000, 0b0010}, // Physical LED #9
      {0b0001, 0b0100}, // Physical LED #8
      {0b0100, 0b0001}, // Physical LED #7
      {0b0100, 0b1000}, // Physical LED #6
      {0b1000, 0b0100}, // Physical LED #5
      {0b0010, 0b0100}, // Physical LED #4
      {0b0100, 0b0010}, // Physical LED #3
      {0b0001, 0b0010}, // Physical LED #2
      {0b0010, 0b0001}, // Physical LED #1
};

/**
 * Turn on given LED
 *
 * @param ledNum
 */
void MotorSimulator::writeLed(unsigned ledNum) {
   unsigned index = ledNum;
   CharliePlexing::write(charlieData[index].highPin);
   CharliePlexing::setDirection(charlieData[index].highPin|charlieData[index].lowPin);
}

/**
 * Motor phases
 */
enum Phase {
   Phase0 = 0b00,
   Phase1 = 0b01,
   Phase2 = 0b10,
   Phase3 = 0b11,
};

/**
 * Called when an illegal motor input sequence has been detected
 */
void MotorSimulator::motorAbort() {
}

/**
 * FTM call-back
 * Polls motor inputs and implements a simple state-machine
 *
 * Sequence
 *
 *         Expected Input
 * Phase   Hold   CW     CCW
 *   0:    0b1000 0b0010 0b0001
 *   1:    0b0010 0b0100 0b1000
 *   2:    0b0100 0b0001 0b0010
 *   3:    0b0001 0b1000 0b0100
 */
void MotorSimulator::timerCallback() {

   static uint8_t  lastInput = 0;

   if (!powerOn) {
      return;
   }

   uint8_t input = MotorPhases::read();

   // Wait until input is stable for 1 sample period
   if (input != lastInput) {
      lastInput = input;
      return;
   }

   // Enable on 1st user input
   enabled = enabled || (input != 0b0000);

   if (!enabled) {
      return;
   }

   Phase phase = (Phase)(motorPosition&0b11);

   switch (phase) {
      default:
      case Phase0:
         if (input == 0b1000) {
            // No action
         }
         else if (input == 0b0010) {
            motorPosition++;
         }
         else if (input == 0b0001) {
            motorPosition--;
         }
         else {
            motorAbort();
         }
         break;
      case Phase1:
         if (input == 0b0010) {
            // No action
         }
         else if (input == 0b0100) {
            motorPosition++;
         }
         else if (input == 0b1000) {
            motorPosition--;
         }
         else {
            motorAbort();
         }
         break;
      case Phase2:
         if (input == 0b0100) {
            // No action
         }
         else if (input == 0b0001) {
            motorPosition++;
         }
         else if (input == 0b0010) {
            motorPosition--;
         }
         else {
            motorAbort();
         }
         break;
      case Phase3:
         if (input == 0b0001) {
            // No action
         }
         else if (input == 0b1000) {
            motorPosition++;
         }
         else if (input == 0b0100) {
            motorPosition--;
         }
         else {
            motorAbort();
         }
         break;
   }
   if (motorPosition == 0) {
      motorPosition = 12;
   }
   if (motorPosition > 12) {
      motorPosition = 1;
   }
   // Update display
   writeLed(motorPosition);
}

/**
 * Test motor LEDs
 */
void MotorSimulator::testMotorLeds() {
   CharliePlexing::setInOut(
         PinPull_None,
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinAction_None,
         PinFilter_None,
         PinSlewRate_Slow);

   motorPosition++;
   if (motorPosition>12) {
      motorPosition = 1;
   }
   writeLed(motorPosition);
}

/**
 * Notification that soft power-on has occurred
 */
void MotorSimulator::softPowerOn() {
   CharliePlexing::setInOut(
         PinPull_None,
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinAction_None,
         PinFilter_None,
         PinSlewRate_Slow);

   MotorPhases::setInput(
         PinPull_None,
         PinAction_None,
         PinFilter_None);

   auto cb = []() {
      This->timerCallback();
   };

   writeLed(enabled?motorPosition:LEDS_OFF);

   MotorPitChannel::setCallback(cb);
   MotorPitChannel::enableNvicInterrupts(NvicPriority_Normal);
   MotorPitChannel::configure(1*ms, PitChannelIrq_Enabled);
}

/**
 * Notification that soft power-off is about to occur
 */
void MotorSimulator::softPowerOff() {
   MotorPitChannel::configure(1*ms, PitChannelIrq_Disabled);
   writeLed(LEDS_OFF);
}

/// Self pointer for static methods e.g. call-backs
MotorSimulator *MotorSimulator::This = nullptr;
