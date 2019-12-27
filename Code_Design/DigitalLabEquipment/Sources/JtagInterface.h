/*
 * JtagInterface.h
 *
 *   Wrapper for JTAG pins being used
 */

#ifndef SOURCES_JTAGINTERFACE_H_
#define SOURCES_JTAGINTERFACE_H_

#include "hardware.h"

class JtagInterface {

private:
//   using Jtag = USBDM::GpioDField<3,0>;
   using Jtag = USBDM::GpioCField<7,4>;
   using Tms  = Jtag::Bit<3>;
   using Tdi  = Jtag::Bit<2>;
   using Tdo  = Jtag::Bit<1>;
   using Tck  = Jtag::Bit<0>;


   using Adc  = USBDM::Adc0;
//   using Vref = Adc::Channel<0>;
   using Vref = Adc::Channel<7>;

   static constexpr unsigned TMS_MASK = (1<<(Tms::BITNUM-Jtag::RIGHT));
   static constexpr unsigned TDI_MASK = (1<<(Tdi::BITNUM-Jtag::RIGHT));
   static constexpr unsigned TDO_MASK = (1<<(Tdo::BITNUM-Jtag::RIGHT));
   static constexpr unsigned TCK_MASK = (1<<(Tck::BITNUM-Jtag::RIGHT));

   static_assert ((Jtag::LEFT-Jtag::RIGHT) == 3);

   static constexpr USBDM::AdcResolution adcResolution = USBDM::AdcResolution_8bit_se;

public:
   /**
    * Enable JTAG interface
    */
   static void enable() {
      using namespace USBDM;

      Jtag::setInOut(
            PinPull_Up, PinDriveStrength_High, PinDriveMode_PushPull,
            PinAction_None, PinFilter_None, PinSlewRate_Fast);
      Jtag::write(0b1111);
      Jtag::setDirection(TMS_MASK|TDI_MASK|TCK_MASK);
   }

   /**
    * Disable JTAG interface
    *
    * @note Pull-ups are still active on pins
    */
   static void disable() {
      Jtag::setIn();
   }

   /**
    * Check for presence of Vref
    *
    * @return true  => Vref present
    * @return false => Vref not present
    */
   static bool checkVref() {
      using namespace USBDM;
      while (Adc0::isBusy()) {
         __asm__("nop");
      }
      Adc::configure(adcResolution, AdcClockSource_Bus);
      return Vref::readAnalogue()>(Adc::getSingleEndedMaximum(adcResolution) * 0.8);
   }

   /**
    * Set TDI pin level
    *
    * @param value
    */
   static void setTDI(bool value) {
      Tdi::write(value);
   }

   /**
    * Set TMS pin level
    *
    * @param value
    */
   static void setTMS(bool value) {
      Tms::write(value);
   }

   /**
    * Cycle TCK pin
    */
   static void clockTCK() {
      Tck::clear();
      Tck::set();
   }

   /**
    * Read TDO pin
    *
    * @return
    */
   static bool getTDO() {
      return Tdo::read();
   }

   /**
    * Cycle TCK / Wait microseconds
    *
    * @param run_test_time Number of clock cycle/microseconds to wait
    */
   static void waitFor(unsigned run_test_time) {
      while(run_test_time-->0) {
         clockTCK();
      }
   }

   /**
    * Wait for microseconds
    *
    * @param microseconds Microseconds to wait
    */
   static void waitUS(unsigned microseconds) {
      USBDM::waitUS(microseconds);
   }
};

#endif /* SOURCES_JTAGINTERFACE_H_ */
