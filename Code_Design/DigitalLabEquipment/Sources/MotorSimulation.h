/*
 * MotorSImulation.h
 *
 *  Created on: 30 Oct 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_MOTORSIMULATION_H_
#define SOURCES_MOTORSIMULATION_H_

#include "hardware.h"
#include "pit.h"

class MotorSimulator {
   static MotorSimulator *This;
   void initialiseMotorSimulation();

public:
   MotorSimulator() {
      USBDM::Pit::configure(USBDM::PitDebugMode_Stop);
      usbdm_assert((This == nullptr), "Only single instance of MotorSimulator allowed");
      This = this;

      initialiseMotorSimulation();
   }

   void testMotorLeds();
   void writeLed(unsigned ledNum);
   void motorAbort();
   void timerCallback();

};

#endif /* SOURCES_MOTORSIMULATION_H_ */
