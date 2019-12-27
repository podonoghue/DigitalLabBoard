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
#include "FunctionQueue.h"
#include "Power.h"

class MotorSimulator : PowerSubscriber {

   /// Self pointer for static methods e.g. call-backs
   static MotorSimulator *This;

   /// Allocated PIT channel
   USBDM::PitChannelNum pitChannel = USBDM::PitChannelNum_None;

   /// Queue for serialised function execution
   FunctionQueue  &functionQueue;

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn() override;

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() override;

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

   /// LED value to turn off all LEDs
   static constexpr unsigned LEDS_OFF = 0;

   /// Whether interface is enabled
   bool enabled = false;

   /// Whether interface is powered
   bool powerOn = false;

   /// Current motor motorPosition
   unsigned motorPosition  = 1;

public:
   /**
    * Constructor
    *
    * @param functionQueue    Serialised call-back function queue
    * @param power            Power object
    */
   MotorSimulator(FunctionQueue &functionQueue, Power &power) : functionQueue(functionQueue) {
      usbdm_assert((This == nullptr), "Only single instance of MotorSimulator allowed");
      This = this;

      power.addPowerSubscriber(this);

      pitChannel = USBDM::Pit::allocateChannel();
   }

   /**
    * Test motor LEDs
    */
   void testMotorLeds();
};

#endif /* SOURCES_MOTORSIMULATION_H_ */
