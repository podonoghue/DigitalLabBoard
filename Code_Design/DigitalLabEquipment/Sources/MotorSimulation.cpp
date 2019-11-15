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
 * Data for 1 Charlie-plexed LED
 */
struct CharlieData {
   uint8_t highLed;
   uint8_t lowLed;
};

/**
 * Data for Charlie-plexed LEDs
 */
static const CharlieData charlieData[] = {
//       3210    3210
      {0b0000, 0b0000}, // Off

      // Visible clockwise sequence
      {0b0001, 0b1000}, // LED #12
      {0b1000, 0b0001}, // LED #11
      {0b0010, 0b1000}, // LED #10
      {0b1000, 0b0010}, // LED #9
      {0b0001, 0b0100}, // LED #8
      {0b0100, 0b0001}, // LED #7
      {0b0100, 0b1000}, // LED #6
      {0b1000, 0b0100}, // LED #5
      {0b0010, 0b0100}, // LED #4
      {0b0100, 0b0010}, // LED #3
      {0b0001, 0b0010}, // LED #2
      {0b0010, 0b0001}, // LED #1
};

/**
 * Turn on given LED
 *
 * @param ledNum
 */
void MotorSimulator::writeLed(unsigned ledNum) {
   unsigned index = ledNum;
   CharliePlexing::write(charlieData[index].highLed);
   CharliePlexing::setDirection(charlieData[index].highLed|charlieData[index].lowLed);
}

/**
 * Motor phases
 */
enum Phase {
   Phase0 = 0b00,//!< Phase0
   Phase1 = 0b01,//!< Phase1
   Phase2 = 0b10,//!< Phase2
   Phase3 = 0b11,//!< Phase3
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

   static unsigned position  = 0;
   static uint8_t  lastInput = 0;

   unsigned lastPosition = position;
   uint8_t input = MotorPhases::read();

   // Wait until input is stable
   if (input != lastInput) {
      lastInput = input;
      return;
   }

   Phase phase = (Phase)(position&0b11);

   switch (phase) {
      default:
      case Phase0:
         if (input == 0b1000) {
            // No action
         }
         else if (input == 0b0010) {
            position++;
         }
         else if (input == 0b0001) {
            position--;
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
            position++;
         }
         else if (input == 0b1000) {
            position--;
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
            position++;
         }
         else if (input == 0b0010) {
            position--;
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
            position++;
         }
         else if (input == 0b0100) {
            position--;
         }
         else {
            motorAbort();
         }
         break;
   }
   if (position == 0) {
      position = 12;
   }
   if (position > 12) {
      position = 1;
   }
   if (position != lastPosition) {
      // Update on change
      writeLed(position);
   }
}

void MotorSimulator::initialiseMotorSimulation() {
   CharliePlexing::setInOut(
         PinPull_None,
         PinDriveStrength_High,
         PinDriveMode_PushPull,
         PinAction_None,
         PinFilter_None,
         PinSlewRate_Slow);
   MotorPhases::setInput(PinPull_None, PinAction_None, PinFilter_None);

   auto cb = []() {
      This->timerCallback();
   };

   MotorPitChannel::setCallback(cb);
   MotorPitChannel::enableNvicInterrupts(NvicPriority_Low);
   MotorPitChannel::configure(1*ms, PitChannelIrq_Enabled);
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
   unsigned position = 0;

   for(;;) {
      position++;
      if (position>12) {
         position = 1;
      }
      writeLed(position);
      waitMS(100);
   }
}

/// Self pointer for static methods e.g. call-backs
MotorSimulator *MotorSimulator::This = nullptr;
