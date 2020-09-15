#include "JtagTables.h" 

struct Transition {
   Xstate via;
   int8_t tms;
};

const Transition fullTapTransitions[16][16] = {
   // -From-      To->Reset             Idle              DR_Select         DR_Capture        DR_Shift          DR_Exit1          DR_Pause          DR_Exit2          DR_Update         IR_Select         IR_Capture        IR_Shift          IR_Exit1          IR_Pause          IR_Exit2          IR_Update         
   /* Reset      */ {{Reset,       1}, {Idle,        0}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Idle,        0}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Idle,        0}, {Illegal,    -1}, {Illegal,    -1}, },
   /* Idle       */ {{DR_Select,   1}, {Idle,        0}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Select  */ {{IR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Capture,  0}, {DR_Capture,  0}, {Illegal,    -1}, {DR_Capture,  0}, {Illegal,    -1}, {Illegal,    -1}, {IR_Select,   1}, {Illegal,    -1}, {IR_Select,   1}, {Illegal,    -1}, {IR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Capture */ {{Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Shift,    0}, {DR_Exit1,    1}, {DR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Shift   */ {{Illegal,    -1}, {DR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Shift,    0}, {DR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Exit1   */ {{Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Pause,    0}, {Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Pause   */ {{DR_Exit2,    1}, {DR_Exit2,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Exit2,    1}, {DR_Exit2,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Exit2,    1}, {Illegal,    -1}, {DR_Exit2,    1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Exit2   */ {{DR_Update,   1}, {DR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Shift,    0}, {Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {DR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* DR_Update  */ {{DR_Select,   1}, {Idle,        0}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* IR_Select  */ {{Reset,       1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Capture,  0}, {IR_Capture,  0}, {Illegal,    -1}, {IR_Capture,  0}, {Illegal,    -1}, {Illegal,    -1}, },
   /* IR_Capture */ {{Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Shift,    0}, {IR_Exit1,    1}, {IR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* IR_Shift   */ {{Illegal,    -1}, {IR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Shift,    0}, {IR_Exit1,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, },
   /* IR_Exit1   */ {{Illegal,    -1}, {IR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Pause,    0}, {Illegal,    -1}, {IR_Update,   1}, },
   /* IR_Pause   */ {{IR_Exit2,    1}, {IR_Exit2,    1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Exit2,    1}, {Illegal,    -1}, {IR_Exit2,    1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Exit2,    1}, {IR_Exit2,    1}, {Illegal,    -1}, },
   /* IR_Exit2   */ {{IR_Update,   1}, {IR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Update,   1}, {Illegal,    -1}, {IR_Update,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {IR_Shift,    0}, {Illegal,    -1}, {IR_Update,   1}, {Illegal,    -1}, {IR_Update,   1}, },
   /* IR_Update  */ {{DR_Select,   1}, {Idle,        0}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {Illegal,    -1}, {DR_Select,   1}, {Illegal,    -1}, {Illegal,    -1}, },
};

/**
 * This is the TMS value to move from one state to another.
 * It incorporates:
 *  - Single transitions between directly connected states
 *  - Transition sequence between stable-states.
 *  - Useful transition sequences IDLE->SHIFT_DR, IDLE->SHIFT_IR
 *
 * 0 => TMS = 0; 1 => TMS = 1; -1 => illegal transition
 */
const int8_t tapTransitions[16][16] = {
   // -From-     To->Reset      Idle       DR_Select  DR_Capture DR_Shift   DR_Exit1   DR_Pause   DR_Exit2   DR_Update  IR_Select  IR_Capture IR_Shift   IR_Exit1   IR_Pause   IR_Exit2   IR_Update  
   /* Reset      */ { 1,         0,        -1,        -1,        -1,        -1,         0,        -1,        -1,        -1,        -1,        -1,        -1,         0,        -1,        -1,        },
   /* Idle       */ { 1,         0,         1,        -1,         1,        -1,         1,        -1,        -1,        -1,        -1,         1,        -1,         1,        -1,        -1,        },
   /* DR_Select  */ { 1,        -1,        -1,         0,         0,        -1,         0,        -1,        -1,         1,        -1,         1,        -1,         1,        -1,        -1,        },
   /* DR_Capture */ {-1,        -1,        -1,        -1,         0,         1,         1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        },
   /* DR_Shift   */ {-1,         1,        -1,        -1,         0,         1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        },
   /* DR_Exit1   */ {-1,         1,        -1,        -1,        -1,        -1,         0,        -1,         1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        },
   /* DR_Pause   */ { 1,         1,        -1,        -1,        -1,        -1,         1,         1,        -1,        -1,        -1,         1,        -1,         1,        -1,        -1,        },
   /* DR_Exit2   */ { 1,         1,        -1,        -1,         0,        -1,         1,        -1,         1,        -1,        -1,         1,        -1,         1,        -1,        -1,        },
   /* DR_Update  */ { 1,         0,         1,        -1,        -1,        -1,         1,        -1,        -1,        -1,        -1,         1,        -1,         1,        -1,        -1,        },
   /* IR_Select  */ { 1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,         0,         0,        -1,         0,        -1,        -1,        },
   /* IR_Capture */ {-1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,         0,         1,         1,        -1,        -1,        },
   /* IR_Shift   */ {-1,         1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,         0,         1,        -1,        -1,        -1,        },
   /* IR_Exit1   */ {-1,         1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,        -1,         0,        -1,         1,        },
   /* IR_Pause   */ { 1,         1,        -1,        -1,         1,        -1,         1,        -1,        -1,        -1,        -1,        -1,        -1,         1,         1,        -1,        },
   /* IR_Exit2   */ { 1,         1,        -1,        -1,         1,        -1,         1,        -1,        -1,        -1,        -1,         0,        -1,         1,        -1,         1,        },
   /* IR_Update  */ { 1,         0,         1,        -1,         1,        -1,         1,        -1,        -1,        -1,        -1,        -1,        -1,         1,        -1,        -1,        },
};

/**
 * State table for JTAG state machine
 * Indicates next-state for each current state and TMS value
 */
const Xstate stateTable[16][2] = {
   // State          TMS=0       TMS=1
   /* Reset      */ {Idle,       Reset     },
   /* Idle       */ {Idle,       DR_Select },
   /* DR_Select  */ {DR_Capture, IR_Select },
   /* DR_Capture */ {DR_Shift,   DR_Exit1  },
   /* DR_Shift   */ {DR_Shift,   DR_Exit1  },
   /* DR_Exit1   */ {DR_Pause,   DR_Update },
   /* DR_Pause   */ {Illegal,    DR_Exit2  },
   /* DR_Exit2   */ {DR_Shift,   DR_Update },
   /* DR_Update  */ {Idle,       DR_Select },
   /* IR_Select  */ {IR_Capture, Reset     },
   /* IR_Capture */ {IR_Shift,   IR_Exit1  },
   /* IR_Shift   */ {IR_Shift,   IR_Exit1  },
   /* IR_Exit1   */ {IR_Pause,   IR_Update },
   /* IR_Pause   */ {Illegal,    IR_Exit2  },
   /* IR_Exit2   */ {IR_Shift,   IR_Update },
   /* IR_Update  */ {Idle,       DR_Select },
};
