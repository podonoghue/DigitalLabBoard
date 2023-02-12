/**
 * @file error.h
 * @brief USBDM Error codes and messages
 *
 *  Created on: 27 Jan 2019
 *      Author: podonoghue
 */

#ifndef INCLUDE_USBDM_ERROR_H_
#define INCLUDE_USBDM_ERROR_H_

extern "C" void _exit(int);

/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
namespace USBDM {

enum ErrorCode {
   E_NO_ERROR = 0,                //!< No error
   E_ERROR,                       //!< General error
   E_TOO_SMALL,                   //!< Value too small
   E_TOO_LARGE,                   //!< Value too large
   E_ILLEGAL_PARAM,               //!< Parameter has illegal value
   E_NO_HANDLER,                  //!< No handler installed
   E_FLASH_INIT_FAILED,           //!< Flash initialisation failed
   E_CALIBRATE_FAIL,              //!< Failed ADC calibration
   E_ILLEGAL_POWER_TRANSITION,    //!< Can't transit to power mode from current power mode
   E_NO_COMMUNICATION,            //!< Failed communication
   E_NO_ACK,                      //!< No acknowledge (I2C)
   E_LOST_ARBITRATION,            //!< Lost arbitration for bus (I2C)
   E_TERMINATED,                  //!< The program has terminated
   E_CLOCK_INIT_FAILED,           //!< Clock initialisation failed
   E_HANDLER_ALREADY_SET,         //!< Handler (callback) already installed
   E_NO_RESOURCE,                 //!< Failed resource allocation
   E_TIMEOUT,                     //!< Timeout occurred during operation
   E_INTERRUPTED,                 //!< Interrupt occurred during operation
   E_BUSY,                        //!< Device is busy

   E_CMSIS_ERR_OFFSET = 1<<20,    //!< Offset added to CMSIS error codes
};

/** Last error set by USBDM code */
extern volatile ErrorCode errorCode;

/**
 * Get USBDM error code.
 * The error code is cleared.
 *
 * @return  E_NO_ERROR - No error
 * @return  other      - Error
 */
inline static ErrorCode getError() {
   return errorCode;
}

/**
 * Get error message from error code or last error if not provided
 *
 * @param[in]   err Error code
 *
 * @return Pointer to static string
 */
const char *getErrorMessage(ErrorCode err = errorCode);

/**
 * Check for error code being set (drastically!)
 * This routine does not return if there is an error
 */
#ifndef DEBUG_BUILD
inline static ErrorCode checkError() {
   while (errorCode != E_NO_ERROR) {
   }
   return errorCode;
}
#else
extern ErrorCode checkError();
#endif

/**
 * Set error code
 *
 * @param[in]  err Error code to set
 *
 * @return Error code
 */
inline static ErrorCode setErrorCode(ErrorCode err) {
   errorCode = err;
   return errorCode;
}

/**
 * Abort program execution with message
 *
 * @param msg  Message to print to console
 *
 * @note Uses a breakpoint rather that exiting the program
 */
extern void abort(const char *msg);

/**
 * Set error code and check for error
 *
 * @param[in]  err Error code to set
 *
 * @return Error code
 */
inline static ErrorCode setAndCheckErrorCode(ErrorCode err) {
   errorCode = err;
   return checkError();
}

/**
 * Clear error code
 */
inline void clearError() {
   errorCode = E_NO_ERROR;
}

#define USE_CONSOLE 0

#if USE_CONSOLE
/**
 * Print simple log message to console
 *
 * @param msg Message to print
 */
extern void log_error(const char * msg);
#else
/**
 * Print simple log message to console\n
 * Dummy for when no console available
 *
 * @param msg Message to print
 */
inline void log_error(const char *) {}
#endif

/**
 * Print simple log message to console
 *
 * @param msg Message to print
 */
inline void _usbdm_assert(const char *msg) {

   USBDM::log_error(msg);
   ::_exit(-1);
}

} // End namespace USBDM

#if !defined (NDEBUG)
#define USBDM_STRINGIFY(x)  #x
#define USBDM_TOSTRING(x)   USBDM_STRINGIFY(x)

/**
 * Macro to do ASSERT operation in debug build
 *
 * @param __e Assert expression to evaluate
 * @param __m Message to print if expression is false
 */
#define USBDM_ASSERT(__e, __m) ((__e) ? (void)0 : (void)USBDM::_usbdm_assert("Assertion Failed @" __FILE__ ":" USBDM_TOSTRING(__LINE__) " - " __m))
#define usbdm_assert(__e, __m) USBDM_ASSERT(__e, __m)
#else
/**
 * Macro to do ASSERT operation in debug build
 *
 * @param __e Assert expression to evaluate
 * @param __m Message to print if expression is false
 */
#define USBDM_ASSERT(__e, __m) ((void)0)
#define usbdm_assert(__e, __m) ((void)0)
#endif

#endif /* INCLUDE_USBDM_ERROR_H_ */
