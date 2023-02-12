/**
 * @file     llwu.h (180.ARM_Peripherals/Project_Headers/llwu.h)
 * @brief    Low leakage wake-up unit
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */

#ifndef HEADER_LLWU_H
#define HEADER_LLWU_H
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "pin_mapping.h"

namespace USBDM {

/**
 * @addtogroup LLWU_Group LLWU, Low-leakage Wake-up Unit
 * @brief Abstraction for Low-leakage Wake-up Unit
 * @{
 */


/**
 * Type definition for LLWU interrupt call back
 */
typedef void (*LlwuCallbackFunction)();

/**
 * Template class providing interface to Low Leakage Wake-up Unit
 *
 * @tparam info      Information class for LLWU
 *
 * @code
 * using llwu = LlwuBase_T<LlwuInfo>;
 *
 *  llwu::configure();
 *
 * @endcode
 */
template <class Info>
class LlwuBase_T : public Info {

protected:
   /** Class to static check llwuPin exists and is mapped to a pin */
   template<int llwuPin> class CheckPinExistsAndIsMapped {
      // Tests are chained so only a single assertion can fail so as to reduce noise

      // Out of bounds value for function index
      static constexpr bool Test1 = (llwuPin>=0) && (llwuPin<(Info::numSignals));
      // Function is not currently mapped to a pin
      static constexpr bool Test2 = !Test1 || (Info::info[llwuPin].gpioBit != UNMAPPED_PCR);
      // Non-existent function and catch-all. (should be INVALID_PCR)
      static constexpr bool Test3 = !Test1 || !Test2 || (Info::info[llwuPin].gpioBit >= 0);

      static_assert(Test1, "Illegal LLWU Input - Check Configure.usbdm for available inputs");
      static_assert(Test2, "LLWU input is not mapped to a pin - Modify Configure.usbdm");
      static_assert(Test3, "LLWU input doesn't exist in this device/package - Check Configure.usbdm for available input pins");

   public:
      /** Dummy function to allow convenient in-line checking */
      static constexpr void check() {}
   };

   /** Callback function for ISR */
   static LlwuCallbackFunction sCallback;

   /** Callback to catch unhandled interrupt */
   static void unhandledCallback() {
      setAndCheckErrorCode(E_NO_HANDLER);
   }

public:
   /**
    * IRQ handler
    */
   static void irqHandler(void) {
      sCallback();
   }

   /**
    * Wrapper to allow the use of a class member as a callback function
    * @note Only usable with static objects.
    *
    * @tparam T         Type of the object containing the callback member function
    * @tparam callback  Member function pointer
    * @tparam object    Object containing the member function
    *
    * @return  Pointer to a function suitable for the use as a callback
    *
    * @code
    * class AClass {
    * public:
    *    int y;
    *
    *    // Member function used as callback
    *    // This function must match LlwuCallbackFunction
    *    void callback(uint32_t status) {
    *       ...;
    *    }
    * };
    * ...
    * // Instance of class containing callback member function
    * static AClass aClass;
    * ...
    * // Wrap member function
    * auto fn = Llwu::wrapCallback<AClass, &AClass::callback, aClass>();
    * // Use as callback
    * Llwu::setCallback(fn);
    * @endcode
    */
   template<class T, void(T::*callback)(), T &object>
   static LlwuCallbackFunction wrapCallback() {
      static LlwuCallbackFunction fn = []() {
         (object.*callback)();
      };
      return fn;
   }

   /**
    * Wrapper to allow the use of a class member as a callback function
    * @note There is a considerable space and time overhead to using this method
    *
    * @tparam T         Type of the object containing the callback member function
    * @tparam callback  Member function pointer
    * @tparam object    Object containing the member function
    *
    * @return  Pointer to a function suitable for the use as a callback
    *
    * @code
    * class AClass {
    * public:
    *    int y;
    *
    *    // Member function used as callback
    *    // This function must match LlwuCallbackFunction
    *    void callback(uint32_t status) {
    *       ...;
    *    }
    * };
    * ...
    * // Instance of class containing callback member function
    * AClass aClass;
    * ...
    * // Wrap member function
    * auto fn = Llwu::wrapCallback<AClass, &AClass::callback>(aClass);
    * // Use as callback
    * Llwu::setCallback(fn);
    * @endcode
    */
   template<class T, void(T::*callback)()>
   static LlwuCallbackFunction wrapCallback(T &object) {
      static T &obj = object;
      static LlwuCallbackFunction fn = []() {
         (obj.*callback)();
      };
      return fn;
   }

   /**
    * Set Callback function
    *
    *   @param[in]  callback Callback function to be executed on interrupt\n
    *                        Use nullptr to remove callback.
    */
   static void setCallback(LlwuCallbackFunction callback) {
      static_assert(Info::irqHandlerInstalled, "LLWU not configured for interrupts");
      if (callback == nullptr) {
         callback = unhandledCallback;
      }
      sCallback = callback;
   }

public:
   /** Pointer to hardware */
   static constexpr HardwarePtr<LLWU_Type> llwu = Info::baseAddress;

   // Template _mapPinsOption.xml (/LLWU/classInfo)

   /**
    * Configures all mapped pins associated with ---Symbol not found or format incorrect for substitution  => key=/LLWU/_base_name, def=null, mod=null
    *
    * @note Locked pins will be unaffected
    */
   static void configureAllPins() {
   
      // Configure pins if selected and not already locked
      if constexpr (Info::mapPinsOnEnable && !(MapAllPinsOnStartup || ForceLockedPins)) {
         Info::initPCRs();
      }
   }

   /**
    * Disabled all mapped pins associated with ---Symbol not found or format incorrect for substitution  => key=/LLWU/_base_name, def=null, mod=null
    *
    * @note Only the lower 16-bits of the PCR registers are modified
    *
    * @note Locked pins will be unaffected
    */
   static void disableAllPins() {
   
      // Disable pins if selected and not already locked
      if constexpr (Info::mapPinsOnEnable && !(MapAllPinsOnStartup || ForceLockedPins)) {
         Info::clearPCRs();
      }
   }

   /**
    * Basic enable of ---Symbol not found or format incorrect for substitution  => key=/LLWU/_base_name, def=null, mod=null
    * Includes enabling clock and configuring all mapped pins if mapPinsOnEnable is selected in configuration
    */
   static void enable() {
      
      configureAllPins();
   }

   /**
    * Disables the clock to ---Symbol not found or format incorrect for substitution  => key=/LLWU/_base_name, def=null, mod=null and all mapped pins
    */
   static void disable() {
      disableNvicInterrupts();
      
      disableAllPins();
      
   }
// End Template _mapPinsOption.xml

   /**
    * Configure with settings from Configure.usbdmProject.
    */
   static void defaultConfigure() {

      // Configure pins
      Info::initPCRs();
      
      // Configure registers
      DefaultInitValue.configure();
      
      enableNvicInterrupts(Info::irqLevel);
   }

   /*
    * ***************************************************
    * Wake-up pins
    * ***************************************************
    */
   /**
    * Configure pin as wake-up source
    *
    * @param[in] llwuPin       Pin to configure
    * @param[in] llwuPinMode   Mode for pin as wake-up input
    */
   static void configurePinSource(
         LlwuPin     llwuPin,
         LlwuPinMode llwuPinMode) {

      static const uint8_t masks[] =
         {LLWU_PE1_WUPE0_MASK, LLWU_PE1_WUPE1_MASK, LLWU_PE1_WUPE2_MASK, LLWU_PE1_WUPE3_MASK, };
      volatile uint8_t &llwuPe = llwu->PE[llwuPin>>2];
      uint8_t mask = masks[llwuPin&3];
      llwuPe = (llwuPe&~mask) | (llwuPinMode&mask);
   }

   /**
    * Get bit mask indicating wake-up pin sources\n
    * The pin masks correspond to Pin sources.
    *
    * @code
    *    if ((pinWakeupSource&(1<<LlwuPin_ptc1)) != 0) {
    *       // Wakeup from LPTMR
    *    }
    * @endcode
    *
    * @return Bit mask
    */
   static uint32_t getPinWakeupSources() {
      constexpr unsigned PF_SIZE = sizeof(llwu->PF)/sizeof(llwu->PF[0]);
      if constexpr(PF_SIZE==4) {
         return (llwu->PF[1]<<24)|(llwu->PF[1]<<16)|(llwu->PF[1]<<8)|llwu->PF[0];
      }
      else if constexpr(PF_SIZE==3) {
         return (llwu->PF[1]<<16)|(llwu->PF[1]<<8)|llwu->PF[0];
      }
      else if constexpr(PF_SIZE==2) {
         return (llwu->PF[1]<<8)|llwu->PF[0];
      }
      else {
         return llwu->PF[0];
      }
   }

   /**
    * Check if pin is source of wake-up
    *
    * @param[in] llwuPin  Pin to check
    *
    * @return false Given pin is not source of wake-up.
    * @return true  Given pin is source of wake-up.
    */
   static bool isPinWakeupSource(LlwuPin llwuPin) {
      return getPinWakeupSources() & (1<<llwuPin);
   }

   /**
    * Clear wake-up pin flag
    *
    *  @param[in] llwuPin Pin indicating which flag to clear
    */
   static void clearPinWakeupFlag(LlwuPin llwuPin) {
      llwu->PF[llwuPin>>3] = (1<<(llwuPin&0x7));
   }

   /**
    * Clear all wake-up flags\n
    * Peripherals sources are not cleared since they are controlled by the peripheral
    */
   static void clearAllFlags() {
      clearPinWakeupFlags();
      clearFilteredPinWakeupFlags();
   }

   /**
    * Clear all wake-up pin flags
    */
   static void clearPinWakeupFlags() {
      for(unsigned index=0; index<(sizeof(llwu->PF)/sizeof(llwu->PF[0])); index++) {
         llwu->PF[index] = 0xFF;
      }
   }

   /*
    * ***************************************************
    * Wake-up filtered pins
    * ***************************************************
    */
   /**
    * Configure one of the input pins as a filtered wake-up source
    *
    * @param[in] llwuFilterNum      Filter to configure - number available depends on device
    * @param[in] llwuPin            Pin to assign to filter
    * @param[in] llwuFilterPinMode  Mode for pin as wake-up input
    *
    * @note Filtering is bypassed in VLLS0
    */
   static ErrorCode configureFilteredPinSource(
         LlwuFilterNum     llwuFilterNum,
         LlwuPin           llwuPin,
         LlwuFilterPinMode llwuFilterPinMode) {

      llwu->FILT[llwuFilterNum] = llwuPin|llwuFilterPinMode;
      return E_NO_ERROR;
   }

   /**
    * Check if filtered wake-up pin is source of wake-up
    *
    * @param[in] filterNum Pin Filter to check
    *
    * @return false Given filtered pin is not source of wake-up.
    * @return true  Given filtered pin is source of wake-up.
    */
   static bool isFilteredPinWakeupSource(unsigned filterNum) {
      return (llwu->FILT[filterNum] & LLWU_FILT_FILTF_MASK);
   }

   /**
    * Clear filtered wake-up pin flag
    *
    * @param[in] filterNum Pin Filter to clear flag
    */
   static void clearFilteredPinWakeupFlag(LlwuFilterNum filterNum) {
      llwu->FILT[filterNum] = llwu->FILT[filterNum] | LLWU_FILT_FILTF_MASK;
   }

   /**
    * Clear all filtered wake-up pin flags
    */
   static void clearFilteredPinWakeupFlags() {
      for (unsigned index=0; index<(sizeof(llwu->FILT)/sizeof(llwu->FILT[0])); index++) {
         llwu->FILT[index] = llwu->FILT[index] | LLWU_FILT_FILTF_MASK;
      }
   }

   /**
    * Disable all wake-up sources (pins and peripherals)
    */
   static void disableAllSources() {
      for (unsigned index=0; index<(sizeof(llwu->PE)/(sizeof(llwu->PE[0]))); index++) {
         llwu->PE[index] = 0;
      }

#if defined(LLWU_ME_WUME0_MASK)
      llwu->ME  = 0;
#endif
   }

   /**
    * Enable interrupts in NVIC
    */
   static void enableNvicInterrupts() {
      NVIC_EnableIRQ(Info::irqNums[0]);
   }

   /**
    * Enable and set priority of interrupts in NVIC
    * Any pending NVIC interrupts are first cleared.
    *
    * @param[in]  nvicPriority  Interrupt priority
    */
   static void enableNvicInterrupts(uint32_t nvicPriority) {
      enableNvicInterrupt(Info::irqNums[0], nvicPriority);
   }

   /**
    * Disable interrupts in NVIC
    */
   static void disableNvicInterrupts() {
      NVIC_DisableIRQ(Info::irqNums[0]);
   }

   template<LlwuPin llwuPin>
   class Pin : public PcrTable_T<Info, llwuPin> {

   private:
      // Checks pin mapping is valid
      LlwuBase_T::CheckPinExistsAndIsMapped<llwuPin> checkPin;

      using Pcr = PcrTable_T<Info, llwuPin>;

      // Hide setOutput
      using Pcr::setOutput;

   public:
      static constexpr LlwuPin  pin = llwuPin;

      /**
       * Constructor
       *
       * @param llwuPinMode LLWU pin wake-up mode
       */
      constexpr Pin(LlwuPinMode llwuPinMode=LlwuPinMode_EitherEdge) {}

      /**
       * Set callback for Pin interrupts
       *
       * @param[in] pinCallback The function to call on Pin interrupt. \n
       *                        nullptr to indicate none
       *
       * @return E_NO_ERROR            No error
       * @return E_HANDLER_ALREADY_SET Handler already set
       *
       * @note There is a single callback function for all pins on the related port.
       *       It is necessary to identify the originating pin in the callback
       */
      static void setPinCallback(PinCallbackFunction pinCallback) {
         static_assert(Pcr::HANDLER_INSTALLED, "Gpio associated with LLWU pin not configured for PIN interrupts - Modify Configure.usbdm");
         Pcr::setPinCallback(pinCallback);
      }
   };

   /**
    * Configure LLWU peripheral wake-up source
    *
    * @param llwuPeripheral       Peripheral used as wake-up source
    * @param llwuPeripheralWakeup Whether this peripheral can wake-up the processor
    */
   static void configurePeripheralSource(
         LlwuPeripheral       llwuPeripheral,
         LlwuPeripheralWakeup llwuPeripheralWakeup) {
   
      if (llwuPeripheralWakeup) {
         llwu->ME = llwu->ME | llwuPeripheral;
      }
      else {
         llwu->ME = llwu->ME & (uint8_t)~llwuPeripheral;
      }
   }
   
   /**
    * Get flag bit mask indicating wake-up peripheral sources
    * The mask returned correspond to (multiple) peripheral sources.
    * These flags are cleared through the originating peripheral.
    *
    *
    * Example checking source
    * @code
    *    if ((peripheralWakeupSource&LlwuPeripheral_Lptmr) != 0) {
    *       // Wake-up from LPTMR
    *    }
    * @endcode
    *
    * @return Bit mask
    */
   static uint32_t getPeripheralWakeupSources() {
      return llwu->MF;
   }

   /**
    *  Check if peripheral is source of wake-up
    *  These flags are cleared through the originating peripheral.
    *
    * @param llwuPeripheral       Peripheral used as wake-up source
    *
    * @return false Given peripheral is not source of wake-up.
    * @return true  Given peripheral is source of wake-up.
    */
   static bool isPeripheralWakeupSource(LlwuPeripheral llwuPeripheral) {
      return llwu->MF & llwuPeripheral;
   }

   /**
    * Controls Reset wake-up control
    *
    * @param llwuResetFilter Enables the digital filter for the RESET pin during LLS, VLLS3, VLLS2, or VLLS1 modes
    * @param llwuResetWakeup This bit must be set to allow the device to be reset while in a low-leakage power mode.
    *        On devices where Reset is not a dedicated pin, the RESET pin must also be enabled
    *        in the explicit port mux control
    */
   static void configureResetFilter(
         LlwuResetFilter llwuResetFilter,
         LlwuResetWakeup llwuResetWakeup = LlwuResetWakeup_Enabled) {
      llwu->RST = llwuResetFilter|llwuResetWakeup;
   }


   // LLWU default init value
   static constexpr LlwuInfo::Init DefaultInitValue {
      LlwuPeripheral_None,  // Peripheral LPTMR0 - Wake-up disabled  
      LlwuPeripheral_None,  // Peripheral CMP0 - Wake-up disabled  
      LlwuPeripheral_None,  // Peripheral CMP1 - Wake-up disabled  
      LlwuPeripheral_None,  //  - Wake-up disabled  
      LlwuPeripheral_None,  // Peripheral TSI0 - Wake-up disabled  
      LlwuPeripheral_None,  // Peripheral RTC_Alarm - Wake-up disabled  
      LlwuPeripheral_None,  //  - Wake-up disabled  
      LlwuPeripheral_None,  // Peripheral RTC_Seconds - Wake-up disabled  
      LlwuPin_0, LlwuPinMode_Disabled,  // Unused input 0 - Wake-up pin disabled, 
      LlwuPin_1, LlwuPinMode_Disabled,  // Unused input 1 - Wake-up pin disabled, 
      LlwuPin_2, LlwuPinMode_Disabled,  // Unused input 2 - Wake-up pin disabled, 
      LlwuPin_3, LlwuPinMode_Disabled,  // Pin PTA4 - Wake-up pin disabled, 
      LlwuPin_4, LlwuPinMode_Disabled,  // Unused input 4 - Wake-up pin disabled, 
      LlwuPin_5, LlwuPinMode_Disabled,  // Pin PTB0 - Wake-up pin disabled, 
      LlwuPin_6, LlwuPinMode_Disabled,  // Pin PTC1 - Wake-up pin disabled, 
      LlwuPin_7, LlwuPinMode_Disabled,  // Pin PTC3 - Wake-up pin disabled, 
      LlwuPin_8, LlwuPinMode_Disabled,  // Pin PTC4 - Wake-up pin disabled, 
      LlwuPin_9, LlwuPinMode_Disabled,  // Pin PTC5 - Wake-up pin disabled, 
      LlwuPin_10, LlwuPinMode_Disabled,  // Pin PTC6 - Wake-up pin disabled, 
      LlwuPin_11, LlwuPinMode_Disabled,  // Unused input 11 - Wake-up pin disabled, 
      LlwuPin_12, LlwuPinMode_Disabled,  // Pin PTD0 - Wake-up pin disabled, 
      LlwuPin_13, LlwuPinMode_Disabled,  // Pin PTD2 - Wake-up pin disabled, 
      LlwuPin_14, LlwuPinMode_Disabled,  // Pin PTD4 - Wake-up pin disabled, 
      LlwuPin_15, LlwuPinMode_Disabled,  // Pin PTD6 - Wake-up pin disabled, 
      LlwuFilterNum_1, LlwuPin_3 , // Filter 1 Pin Select - Pin PTA4
      LlwuFilterPinMode_Disabled,  // Wake-up On External Pin with Digital Filter - Wake-up disabled, 
      LlwuFilterNum_2, LlwuPin_3 , // Filter 2 Pin Select - Pin PTA4
      LlwuFilterPinMode_Disabled,  // Wake-up On External Pin with Digital Filter - Wake-up disabled, 
      LlwuResetWakeup_Enabled, LlwuResetFilter_Disabled,  
   };



};

template<class Info> LlwuCallbackFunction LlwuBase_T<Info>::sCallback = LlwuBase_T<Info>::unhandledCallback;

   /**
    * Class representing LLWU
    */
   class Llwu : public LlwuBase_T<LlwuInfo> {};

/**
 * End LLWU_Group
 * @}
 */

} // End namespace USBDM

#endif /* HEADER_LLWU_H */
