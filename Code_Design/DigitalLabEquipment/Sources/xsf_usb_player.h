/*
 * xsf_usb_player.h
 *
 *  Created on: 10 Jan 2020
 *      Author: podonoghue
 */

#ifndef SOURCES_XSF_USB_PLAYER_H_
#define SOURCES_XSF_USB_PLAYER_H_
#include "Configure.h"
#include "Power.h"

class UsbXsvfInterface : PowerSubscriber {
private:
   enum UsbState {UsbStartUp, UsbIdle, UsbWaiting};

   static UsbCommandMessage command;

   /**
    * Notification that soft power-on has occurred
    */
   virtual void softPowerOn() override {
   }

   /**
    * Notification that soft power-off is about to occur
    */
   virtual void softPowerOff() override {
      using namespace USBDM;

      // Just turn off the programmer LEDs
      ProgrammerFailLed::off();
      ProgrammerPassLed::off();
   }

   /**
    * Read IDCODE from CPLD
    *
    * @param[out] idcode  IDCODE as 32-bit integer
    *
    * @return false  => Success. IDCODE has value
    * @return true   => Failed to read IDCODE
    */
   static bool readIdcode(uint32_t &idcode);

   /** For debug
    *
    * @param xsvd_size
    * @param xsvf_data
    * @param result
    * @return
    */
   static bool execute(unsigned xsvd_size, uint8_t *xsvf_data, uint32_t &result);

public:

   UsbXsvfInterface(Power &power) {
      using namespace USBDM;

      // Initialise hardware
      ProgrammerFailLed::setOutput(
            PinDriveStrength_High,
            PinDriveMode_PushPull,
            PinSlewRate_Slow);

      ProgrammerPassLed::setOutput(
            PinDriveStrength_High,
            PinDriveMode_PushPull,
            PinSlewRate_Slow);

      power.addPowerSubscriber(this);
   }

   ~UsbXsvfInterface() {
   }

   /**
    * Poll USB interface for activity
    *
    * If active the return may be very much delayed e.g. programming of a device.
    */
   void pollUsb();

};

#endif /* SOURCES_XSF_USB_PLAYER_H_ */
