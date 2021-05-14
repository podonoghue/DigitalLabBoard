/**
 * @file     smc.h (180.ARM_Peripherals/Project_Headers/smc.h)
 * @brief    System Management Controller
 *
 * @version  V4.12.1.210
 * @date     13 April 2016
 */

#ifndef HEADER_SMC_H
#define HEADER_SMC_H
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "hardware.h"

namespace USBDM {

/**
 * @addtogroup SMC_Group SMC, System Mode Controller
 * @brief Abstraction for System Mode Controller
 * @{
 */

/**
 * Determines if VLPR, VLPW, and VLPS modes are enabled
 */
enum SmcVeryLowPower {
   SmcVeryLowPower_Disabled  = SMC_PMPROT_AVLP(0),   //!< Disallow VLPR, VLPW, and VLPS modes
   SmcVeryLowPower_Enabled   = SMC_PMPROT_AVLP(1),   //!< Allow VLPR, VLPW, and VLPS modes
};

#ifdef SMC_PMPROT_ALLS
/**
 * Determines if any LLSx modes are enabled
 *
 * @note Not supported on all processors
 */
enum SmcLowLeakageStop {
   SmcLowLeakageStop_Disabled  = SMC_PMPROT_ALLS(0),   //!< Disallow Any LLSx mode
   SmcLowLeakageStop_Enabled   = SMC_PMPROT_ALLS(1),   //!< Allow Any LLSx mode
};
#endif

#ifdef SMC_PMPROT_AVLLS
/**
 * Determines if any VLLSx modes are enabled
 *
 * @note Not supported on all processors
 */
enum SmcVeryLowLeakageStop {
   SmcVeryLowLeakageStop_Disabled  = SMC_PMPROT_AVLLS(0),   //!< Disallow Any VLLSx mode
   SmcVeryLowLeakageStop_Enabled   = SMC_PMPROT_AVLLS(1),   //!< Allow Any VLLSx mode
};
#endif

#ifdef SMC_PMPROT_AHSRUN
/**
 * Determines if HSRUN mode is enabled
 *
 * @note Not supported on all processors
 */
enum SmcHighSpeedRun {
   SmcHighSpeedRun_Disabled  = SMC_PMPROT_AHSRUN(0),   //!< Disallow HSRUN mode
   SmcHighSpeedRun_Enabled   = SMC_PMPROT_AHSRUN(1),   //!< Allow HSRUN mode
};
#endif

#ifdef SMC_PMCTRL_LPWUI
/**
 * Whether to exit VLP to Run mode on interrupt
 *
 * @note Not supported on all processors
 */
enum SmcExitVeryLowPowerOnInt {
   SmcExitVeryLowPowerOnInt_Disabled = SMC_PMCTRL_LPWUI(0),  //!< Remain in VLP mode on interrupt
   SmcExitVeryLowPowerOnInt_Enabled  = SMC_PMCTRL_LPWUI(1),  //!< Exit to RUN mode on interrupt
};
#endif

/**
 * Sets Run mode
 *
 * @note Not all modes are supported on all processors
 */
enum SmcRunMode {
   SmcRunMode_Normal       = SMC_PMCTRL_RUNM(0),   //!< Normal run mode (RUN)
   SmcRunMode_VeryLowPower = SMC_PMCTRL_RUNM(2),   //!< Very low power run mode (VLPR)
#ifdef SMC_PMPROT_AHSRUN
   SmcRunMode_HighSpeed    = SMC_PMCTRL_RUNM(3),   //!< High Speed Run mode (HSRUN)
#endif
};

/**
 * Sets Stop mode
 */
enum SmcStopMode {
   SmcStopMode_NormalStop         = SMC_PMCTRL_STOPM(0), //!< Normal Stop (STOP)
   SmcStopMode_VeryLowPowerStop   = SMC_PMCTRL_STOPM(2), //!< Very-Low-Power Stop (VLPS)
#ifdef SMC_PMPROT_ALLS_MASK
   SmcStopMode_LowLeakageStop     = SMC_PMCTRL_STOPM(3), //!< Low-Leakage Stop (LLSx)
#endif
#ifdef SMC_PMPROT_AVLLS_MASK
   SmcStopMode_VeryLowLeakageStop = SMC_PMCTRL_STOPM(4), //!< Very-Low-Leakage Stop (VLLSx)
#endif
};

/**
 * Control power to RAM2 in LLS2/VLLS2 power mode
 */
#ifdef SMC_STOPCTRL_RAM2PO_MASK
enum SmcLowLeakageRam2 {
   SmcLowLeakageRam2_Disabled = SMC_STOPCTRL_RAM2PO(0),  //! RAM2 not powered in LLS2/VLLS2
   SmcLowLeakageRam2_Enabled  = SMC_STOPCTRL_RAM2PO(1),  //! RAM2 powered in LLS2/VLLS2
};
#endif

#ifdef SMC_STOPCTRL_PSTOPO
/**
 *  Partial Stop Option\n
 *  Controls whether a Partial Stop mode is entered when STOPM=STOP\n
 *
 * @note Not supported on all processors
 */
enum SmcPartialStopMode {
   SmcPartialStopMode_Normal   = SMC_STOPCTRL_PSTOPO(0), //!< Normal stop mode
   SmcPartialStopMode_Partial1 = SMC_STOPCTRL_PSTOPO(1), //!< Partial Stop with both system and bus clocks disabled
   SmcPartialStopMode_Partial2 = SMC_STOPCTRL_PSTOPO(2), //!< Partial Stop with system clock disabled and bus clock enabled

   SmcPartialStopMode_Stop = SmcPartialStopMode_Normal,    //!< Normal stop mode
   SmcPartialStopMode_Stop1 = SmcPartialStopMode_Partial1, //!< Stop with both system and bus clocks disabled
   SmcPartialStopMode_Stop2 = SmcPartialStopMode_Partial2, //!< Stop with system clock disabled and bus clock enabled

};
#endif

#ifdef SMC_STOPCTRL_LPOPO
/**
 *  Stop mode LPO Option\n
 *  Controls whether the 1 kHz LPO clock is enabled in LLS/VLLSx modes.
 *
 * @note Not supported on all processors
 */
enum SmcLpoInLowLeakage {
   SmcLpoInLowLeakage_Enabled  = SMC_STOPCTRL_LPOPO(0), //!< LPO clock is enabled in LLS/VLLSx
   SmcLpoInLowLeakage_Disabled = SMC_STOPCTRL_LPOPO(1), //!< LPO clock is disabled in LLS/VLLSx
};
#endif

#ifdef SMC_STOPCTRL_PORPO
/**
 *  POR Power Option\n
 *  This bit controls whether the POR detect circuit is enabled in VLLS0 mode.
 */
enum SmcPowerOnReset {
   SmcPowerOnReset_Enabled  = SMC_STOPCTRL_PORPO(0),   //!< Power on reset (brown-out detection) in VLLS0 enabled
   SmcPowerOnReset_Disabled = SMC_STOPCTRL_PORPO(1),   //!< Power on reset (brown-out detection) in VLLS0 disabled
};
#endif

/**
 *  Sleep on exit from Interrupt Service Routine (ISR)\n
 *  This option controls whether the processor re-enters sleep mode when exiting the\n
 *  handler for the interrupt that awakened it.
 */
enum SmcSleepOnExit {
   SmcSleepOnExit_Disabled = 0,                       //!< Processor does not re-enter SLEEP/DEEPSLEEP mode on completion of interrupt.
   SmcSleepOnExit_Enabled  = SCB_SCR_SLEEPONEXIT_Msk, //!< Processor re-enters SLEEP/DEEPSLEEP mode on completion of interrupt.
};

#if defined(SMC_STOPCTRL_LLSM) || defined(SMC_STOPCTRL_VLLSM)
/**
 *  VLS or VLLS Mode Control\n
 *  This field controls which LLS/VLLS sub-mode to enter if STOPM=LLS/VLLS
 *
 * @note Not all modes are supported on all processors
 */
enum SmcLowLeakageStopMode {
#ifdef SMC_STOPCTRL_LLSM
   SmcLowLeakageStopMode_VLLS0 = SMC_STOPCTRL_LLSM(0),  //!< Enter VLLS0 in VLLSx mode
   SmcLowLeakageStopMode_VLLS1 = SMC_STOPCTRL_LLSM(1),  //!< Enter VLLS1 in VLLSx mode
   SmcLowLeakageStopMode_VLLS2 = SMC_STOPCTRL_LLSM(2),  //!< Enter VLLS2 in VLLSx mode, LLS2 in LLSx mode
   SmcLowLeakageStopMode_VLLS3 = SMC_STOPCTRL_LLSM(3),  //!< Enter VLLS3 in VLLSx mode, LLS3 in LLSx mode
   SmcLowLeakageStopMode_LLS2  = SMC_STOPCTRL_LLSM(2),  //!< Enter VLLS2 in VLLSx mode, LLS2 in LLSx mode
   SmcLowLeakageStopMode_LLS3  = SMC_STOPCTRL_LLSM(3),  //!< Enter VLLS3 in VLLSx mode, LLS3 in LLSx mode
#else
   SmcLowLeakageStopMode_VLLS0 = SMC_STOPCTRL_VLLSM(0),  //!< Enter VLLS0 in VLLSx mode
   SmcLowLeakageStopMode_VLLS1 = SMC_STOPCTRL_VLLSM(1),  //!< Enter VLLS1 in VLLSx mode
   SmcLowLeakageStopMode_VLLS2 = SMC_STOPCTRL_VLLSM(2),  //!< Enter VLLS2 in VLLSx mode, LLS2 in LLSx mode
   SmcLowLeakageStopMode_VLLS3 = SMC_STOPCTRL_VLLSM(3),  //!< Enter VLLS3 in VLLSx mode, LLS3 in LLSx mode
#endif
};
#endif

/**
 *  Indicates the current stop mode
 */
enum SmcStatus {
   // Run modes
#ifdef SMC_PMPROT_AHSRUN
   SmcStatus_HSRUN  = SMC_PMSTAT_PMSTAT(1<<7),    //!< Processor is in High Speed Run mode
#endif
   SmcStatus_RUN    = SMC_PMSTAT_PMSTAT(1<<0),    //!< Processor is in Normal Run mode
   SmcStatus_VLPR   = SMC_PMSTAT_PMSTAT(1<<2),    //!< Processor is in Very Low Power Run mode

   // Sleep = Wait modes
   SmcStatus_VLPW   = SMC_PMSTAT_PMSTAT(1<<3),    //!< Processor is in Very Low Power Wait mode

   // Stop = DeepSleep modes
   SmcStatus_STOP   = SMC_PMSTAT_PMSTAT(1<<1),    //!< Processor is in Stop mode
   SmcStatus_VLPS   = SMC_PMSTAT_PMSTAT(1<<4),    //!< Processor is in Very Low Power Stop mode
   SmcStatus_LLS    = SMC_PMSTAT_PMSTAT(1<<5),    //!< Processor is in Low Leakage Stop mode
   SmcStatus_VLLS   = SMC_PMSTAT_PMSTAT(1<<6),    //!< Processor is in Very Low Leakage Stop mode
};

/**
 * @brief Template class representing the System Mode Controller (SMC)
 *
 * Partially based on Freescale Application note AN4503\n
 * Support for Kinetis Low Power operation.
 *
 * @image html KinetisPowerModes.png
 */
template <class Info>
class SmcBase_T {

protected:
	   /** Hardware instance pointer */
	   static __attribute__((always_inline)) volatile SMC_Type &smc() { return Info::smc(); }

public:

   /**
    * Get name from SMC status e.g. RUN, VLPR, HSRUN
    *
    * @param status
    *
    * @return Pointer to static string
    */
   static const char *getSmcStatusName(SmcStatus status) {
#ifdef SMC_PMPROT_AHSRUN
      if (status == SmcStatus_HSRUN) {
         return "HSRUN";
      }
#endif
      if (status == SmcStatus_RUN) {
         return "RUN";
      }
      if (status == SmcStatus_VLPR) {
         return "VLPR";
      }
      return "Impossible while running!";
   }
   
   /**
    * Get name for current SMC status  e.g. RUN, VLPR, HSRUN
    *
    * @return Pointer to static string
    */
   static const char *getSmcStatusName() {
      return getSmcStatusName(getStatus());
   }
   /**
    * Basic enable of SMC\n
    * Includes configuring all pins
    */
   static __attribute__((always_inline)) void enable() {
      // No clock or pins
   }

   /**
    * Configure with settings from <b>Configure.usbdmProject</b>.
    */
   static void defaultConfigure() {
      smc().PMPROT   = Info::pmprot;
      smc().STOPCTRL = Info::stopctrl;
   }
   
   /* smc_mk10d5.xml */
   /**
    * Enable the given power modes.
    * A mode must be enabled before it can be entered.
    *
    * @param[in] smcVeryLowPower        Allows VLPR, VLPW, and VLPS modes
    * @param[in] smcLowLeakageStop      Allows LLSx modes
    * @param[in] smcVeryLowLeakageStop  Allows VLLSx modes
    *
    * @note This is a write-once-after-reset operation
    */
   static ErrorCode enablePowerModes(
         SmcVeryLowPower         smcVeryLowPower,
         SmcLowLeakageStop       smcLowLeakageStop       = SmcLowLeakageStop_Disabled,
         SmcVeryLowLeakageStop   smcVeryLowLeakageStop   = SmcVeryLowLeakageStop_Disabled) {
   
      smc().PMPROT = smcVeryLowPower|smcLowLeakageStop|smcVeryLowLeakageStop;
      return E_NO_ERROR;
   }


   /* smc_mk10d5.xml */
   /**
    * Allows the detailed operation in STOP mode to be controlled.
    *
    * @param[in] smcLowLeakageStopMode  Controls which LLS/VLLS sub-mode to enter if STOPM=LLS/VLLS
    * @param[in] smcPowerOnReset        Controls whether the POR detect circuit is enabled in VLLS0 mode
    */
   static void setStopOptions(
         SmcLowLeakageStopMode   smcLowLeakageStopMode,
         SmcPowerOnReset         smcPowerOnReset         = SmcPowerOnReset_Disabled ) {
   
      smc().STOPCTRL = smcPowerOnReset|smcLowLeakageStopMode;
   }


   /**
    * Get current power status
    *
    * @return SmcStatus value indicating operating mode
    */
   static SmcStatus getStatus() {

      return static_cast<SmcStatus>(smc().PMSTAT);
   }

   /**
    * Enter Run Mode.
    *
    * This may be used to change between supported RUN modes (RUN, VLPR, HSRUN).\n
    * Only the following transitions are allowed: VLPR <-> RUN <-> HSRUN.
    *
    * @param[in] smcRunMode  Mode to enter
    *
    * @return E_NO_ERROR                 No error
    * @return E_ILLEGAL_POWER_TRANSITION Cannot transition to smcRunMode from current run mode
    */
   static ErrorCode enterRunMode(SmcRunMode smcRunMode) {
#ifdef SMC_PMPROT_AHSRUN
      SmcStatus smcStatus = getStatus();
#endif
      switch(smcRunMode) {
         case SmcRunMode_Normal:
            smc().PMCTRL = (smc().PMCTRL&~SMC_PMCTRL_RUNM_MASK)|smcRunMode;
            // Wait for power status to change
            while (getStatus() != SmcStatus_RUN) {
               __asm__("nop");
            }
            break;
#ifdef SMC_PMPROT_AHSRUN
         case SmcRunMode_HighSpeed:
            if (smcStatus != SmcStatus_RUN) {
               // Can only transition from RUN mode
               return setErrorCode(E_ILLEGAL_POWER_TRANSITION);
            }
            smc().PMCTRL = (smc().PMCTRL&~SMC_PMCTRL_RUNM_MASK)|smcRunMode;
            // Wait for power status to change
            while (getStatus() != SmcStatus_HSRUN) {
               __asm__("nop");
            }
            break;
#endif
         case SmcRunMode_VeryLowPower:
#ifdef SMC_PMPROT_AHSRUN
            if (smcStatus != SmcStatus_RUN) {
               // Can only transition from RUN mode
               return setErrorCode(E_ILLEGAL_POWER_TRANSITION);
            }
#endif
            smc().PMCTRL = (smc().PMCTRL&~SMC_PMCTRL_RUNM_MASK)|smcRunMode;
            // Wait for power status to change
            while (getStatus() != SmcStatus_VLPR) {
               __asm__("nop");
            }
            break;
         default:
            return setErrorCode(E_ILLEGAL_PARAM);
      }
#ifdef USBDM_SCG_IS_DEFINED
      // Update clocks as clock change is automatic with run mode change
      // Update clocks as clock change is automatic
      Scg::SystemCoreClockUpdate();
#endif
      return E_NO_ERROR;
   }

   /**
    * Set Stop Mode to enter on Sleep-Now or Sleep-On-Exit
    *
    * @param[in]  smcStopMode Stop mode to set
    */
   static void setStopMode(SmcStopMode smcStopMode) {
      smc().PMCTRL = (smc().PMCTRL&~SMC_PMCTRL_STOPM_MASK)|smcStopMode;
      // Make sure write completes
      __DSB();
   }

   /**
    * Enter Stop Mode (STOP, VLPS, LLSx, VLLSx)
    * (ARM core DEEPSLEEP mode)
    *
    * The processor will stop execution and enter the currently configured STOP mode.\n
    * Peripherals affected will depend on the stop mode selected.\n
    * The stop mode to enter may be set by setStopMode().
    * Other options that affect stop mode may be set by setStopOptions().
    */
   static void enterStopMode() {
      SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
      // Make sure write completes
      (void)(SCB->SCR);
      __asm volatile( "dsb" ::: "memory" );
      __asm volatile( "wfi" );
      __asm volatile( "isb" );
   }

   /**
    * Enter Stop Mode (STOP, VLPS, LLSx, VLLSx)
    * (ARM core DEEPSLEEP mode)
    *
    * The processor will stop execution and enter the given STOP mode.\n
    * Peripherals affected will depend on the stop mode selected.
    *
    * @param[in] smcStopMode Stop mode to set.  This will become the default STOP mode.
    */
   static void enterStopMode(SmcStopMode smcStopMode) {
      setStopMode(smcStopMode);
      enterStopMode();
   }

   /**
    * Enter Deep Sleep mode with the current STOP settings
    *
    * See enterStopMode();
    */
   static void deepSleep() {
      enterStopMode();
   }

   /**
    * Enter Wait Mode (WAIT, VLPW)\n
    * (ARM core SLEEP mode)
    *
    * The processor will stop execution and enter WAIT/VLPW mode.\n
    * This function can be used to enter normal WAIT mode or VLPW mode
    * depending upon current run mode.\n
    * In wait mode the core clock is disabled (no code executing),
    * but bus clocks are enabled (peripheral modules are operational).
    *
    * Possible power mode transitions:
    * - RUN  -> WAIT
    * - VLPR -> VLPW
    *
    * WAIT mode is exited using any enabled interrupt or RESET.
    *
    * For Kinetis K:
    * If in VLPW mode, the statue of the SMC_PMCTRL[LPWUI] bit
    * determines if the processor exits to VLPR or RUN mode.\n
    * Use setExitVeryLowPowerOnInterrupt() to modify this action.
    *
    * For Kinetis L:
    * LPWUI does not exist.\n
    * Exits with an interrupt from VLPW will always be back to VLPR.\n
    * Exits from an interrupt from WAIT will always be back to RUN.
    *
    * @note Some modules include a programmable option to disable them in wait mode.\n
    * If those modules are programmed to disable in wait mode, they will not be able to
    * generate interrupts to wake the core.
    */
   static void enterWaitMode() {
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
      // Make sure write completes
      (void)(SCB->SCR);
      __asm volatile( "dsb" ::: "memory" );
      __asm volatile( "wfi" );
      __asm volatile( "isb" );
   }

   /**
    * Enter SLEEP mode
    *
    * See enterWaitMode();
    */
   static void sleep() {
      enterWaitMode();
   }

#ifdef SMC_PMCTRL_LPWUI_MASK
   /**
    * Select VLP action on interrupt when in VLP modes (VLPR, VLPW or VLPS).
    *
    * @param[in]  smcExitVeryLowPowerOnInt Whether to exit from any VLP mode to RUN mode on interrupt
    *
    * @return E_NO_ERROR                 Success
    * @return E_ILLEGAL_POWER_TRANSITION If not in RUN mode
    */
   static ErrorCode setExitVeryLowPowerOnInterrupt(SmcExitVeryLowPowerOnInt smcExitVeryLowPowerOnInt) {
      if (getStatus() != SmcStatus_RUN) {
         // Can only change in RUN mode
         return setErrorCode(E_ILLEGAL_POWER_TRANSITION);
      }
      smc().PMCTRL = (smc().PMCTRL&SMC_PMCTRL_STOPM_MASK) | smcExitVeryLowPowerOnInt;
      // Make sure write completes
      (void)smc().PMCTRL;
      return E_NO_ERROR;
   }
#endif

   /**
    * Set Sleep-on-exit action
    *
    * If enabled, when the processor completes the execution of all exception handlers it
    * returns to Thread mode and immediately enters WAIT/STOP mode (ARM core SLEEP/DEEPSLEEP mode).\n
    * Use this mechanism in applications that only require the processor to run when
    * an exception occurs.
    *
    * @param[in] smcSleepOnExit Determines action on completion of all exception handlers
    */
   static void setSleepOnExit(SmcSleepOnExit smcSleepOnExit=SmcSleepOnExit_Enabled) {
      if (smcSleepOnExit) {
         SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
      }
      else {
         SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
      }
      // Make sure write completes
      (void)(SCB->SCR);
   }
};

#ifdef USBDM_SMC_IS_DEFINED
/**
 * Class representing SMC
 */
class Smc : public SmcBase_T<SmcInfo> {};

#endif

/**
 * End SMC_Group
 * @}
 */

} // End namespace USBDM

#endif /* HEADER_SMC_H */
