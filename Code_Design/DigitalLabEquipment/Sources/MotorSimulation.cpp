/*
 * MotorSImulation.cpp
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */

#include <MotorSimulation.h>
#include "Configure.h"
#include "pit.h"

using namespace USBDM;

using Timer = Pit;
using TimerChannel = Timer::Channel<0>;

struct CharlieData {
   uint8_t highLed;
   uint8_t lowLed;
};

static CharlieData charlieData[] = {
//       3210    3210
      {0b0000, 0b0000}, // Off
      {0b0010, 0b0001}, // LED #1
      {0b0001, 0b0010},
      {0b0100, 0b0010},
      {0b0010, 0b0100},

      {0b1000, 0b0100},
      {0b0100, 0b1000},
      {0b0010, 0b1000},
      {0b1000, 0b0010},

      {0b0001, 0b0100},
      {0b0100, 0b0001},
      {0b0001, 0b1000},
      {0b1000, 0b0001}, // LED #12
};

/**
 *
 * @param ledNum
 */
static void writeLed(unsigned ledNum) {
   unsigned index = ledNum + 1;
   CharliePlexing::write(charlieData[index].highLed);
   CharliePlexing::setDirection(charlieData[index].highLed|charlieData[index].lowLed);
}

enum Phase {
   Phase0 = 0b00,
   Phase1 = 0b01,
   Phase2 = 0b10,
   Phase3 = 0b11,
};

static void motorAbort() {

}

/*
 * Sequence
 *
 * 0: 0b1000 0b0010 0b0001
 * 1: 0b0010 0b0100 0b1000
 * 2: 0b0100 0b0001 0b0010
 * 3: 0b0001 0b1000 0b0100
 */
static void timerCallback() {

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
   if (position >= 12) {
      position = 0;
   }
   if (position != lastPosition) {
      writeLed(position);
   }
}

void initialiseMotorSimulation() {
   Timer::configure(PitDebugMode_Stop);
   TimerChannel::setCallback(timerCallback);
   TimerChannel::enableNvicInterrupts(NvicPriority_Low);
   TimerChannel::configure(1*ms, PitChannelIrq_Enabled);
}
