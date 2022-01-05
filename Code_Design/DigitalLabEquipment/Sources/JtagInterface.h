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
   static constexpr unsigned TMS_MASK = (1<<(USBDM::Tms::BITNUM-USBDM::Jtag::RIGHT));
   static constexpr unsigned TDI_MASK = (1<<(USBDM::Tdi::BITNUM-USBDM::Jtag::RIGHT));
   static constexpr unsigned TDO_MASK = (1<<(USBDM::Tdo::BITNUM-USBDM::Jtag::RIGHT));
   static constexpr unsigned TCK_MASK = (1<<(USBDM::Tck::BITNUM-USBDM::Jtag::RIGHT));

   static_assert ((USBDM::Jtag::LEFT-USBDM::Jtag::RIGHT) == 3);

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
      Jtag::bitWrite(0b1111);
      Jtag::setDirection(TMS_MASK|TDI_MASK|TCK_MASK);
   }

   /**
    * Disable JTAG interface
    *
    * @note Pull-ups are still active on pins
    */
   static void disable() {
      using namespace USBDM;
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
      while (MyAdc::isBusy()) {
         __asm__("nop");
      }
      MyAdc::configure(adcResolution, AdcClockSource_Bus);
      return JtagVref::readAnalogue()>(MyAdc::getSingleEndedMaximum(adcResolution) * 0.8);
   }

   /**
    * Set TDI pin level
    *
    * @param value
    */
   static void setTDI(bool value) {
      using namespace USBDM;
      Tdi::writeBit(value);
   }

   /**
    * Set TMS pin level
    *
    * @param value
    */
   static void setTMS(bool value) {
      using namespace USBDM;
      Tms::writeBit(value);
   }

   /**
    * Cycle TCK pin
    */
   static void clockTCK() {
      using namespace USBDM;
      Tck::clear();
      Tck::set();
   }

   /**
    * Read TDO pin
    *
    * @return
    */
   static bool getTDO() {
      using namespace USBDM;
      return Tdo::readBit();
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
