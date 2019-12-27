/*
 * JtagTables.h
 *
 *  Created on: 16 Dec 2019
 *      Author: podonoghue
 */

#ifndef JTAGTABLES_H_
#define JTAGTABLES_H_

#include <stdint.h>

/// JTAG states
enum Xstate : uint8_t {
   Reset            = 0x00, //!< Reset
   Idle             = 0x01, //!< Idle
   DR_Select        = 0x02, //!< DR_Select
   DR_Capture       = 0x03, //!< DR_Capture
   DR_Shift         = 0x04, //!< DR_Shift
   DR_Exit1         = 0x05, //!< DR_Exit1
   DR_Pause         = 0x06, //!< DR_Pause
   DR_Exit2         = 0x07, //!< DR_Exit2
   DR_Update        = 0x08, //!< DR_Update
   IR_Select        = 0x09, //!< IR_Select
   IR_Capture       = 0x0A, //!< IR_Capture
   IR_Shift         = 0x0B, //!< IR_Shift
   IR_Exit1         = 0x0C, //!< IR_Exit1
   IR_Pause         = 0x0D, //!< IR_Pause
   IR_Exit2         = 0x0E, //!< IR_Exit2
   IR_Update        = 0x0F, //!< IR_Update
   Illegal          = 255,  //!< Illegal
};

extern const int8_t tapTransitions[16][16];

extern const Xstate stateTable[16][2];

#endif /* JTAGTABLES_H_ */
