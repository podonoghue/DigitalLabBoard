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

   /// Self pointer for static methods e.g. call-backs
   static MotorSimulator *This;

   /**
    * Initialises the module
    */
   void initialiseMotorSimulation();

public:
   /**
    * Constructor
    */
   MotorSimulator() {
      usbdm_assert((This == nullptr), "Only single instance of MotorSimulator allowed");
      This = this;

      USBDM::Pit::configure(USBDM::PitDebugMode_Stop);

      initialiseMotorSimulation();
   }

   /**
    * Turn on given LED
    *
    * @param ledNum
    */
   void writeLed(unsigned ledNum);

   /**
    * Called when an illegal motor input sequence has been detected
    */
   void motorAbort();

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
   void timerCallback();

   /**
    * Test motor LEDs
    */
   void testMotorLeds();
};

#endif /* SOURCES_MOTORSIMULATION_H_ */
