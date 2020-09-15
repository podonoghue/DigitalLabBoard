/**============================================================================
 * @file GenerateTables.cpp
 *
 * Generate JTAG transition files
 *============================================================================*/

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "JtagTables.h"

struct Transition {
   Xstate via;
   int8_t tms;
};

inline Xstate &operator++ (Xstate& d) {
    d = static_cast<Xstate>((static_cast<int>(d) + 1));
    return d;
}

const char *getXstateName(Xstate state) {
   static const char* names[] = {
         "Reset",
         "Idle",
         "DR_Select",
         "DR_Capture",
         "DR_Shift",
         "DR_Exit1",
         "DR_Pause",
         "DR_Exit2",
         "DR_Update",
         "IR_Select",
         "IR_Capture",
         "IR_Shift",
         "IR_Exit1",
         "IR_Pause",
         "IR_Exit2",
         "IR_Update",
   };
   const char *name = "Illegal";
   if (((unsigned)state)<(sizeof(names)/sizeof(names[0]))) {
      name = names[state];
   }
   return (name);
}

const char *getXstateNameComma(Xstate state) {
   static char name[20];
   strcpy(name,getXstateName(state));
   strcat(name,",");
   return name;
}

Transition transitions[16][16];

void addTransition(Xstate from, Xstate to, Xstate via, int8_t tms) {
   if ((transitions[from][to].via != Illegal) && (via != Illegal) &&
         ((transitions[from][to].via != via) || (transitions[from][to].tms != tms)) ) {
      printf("%s -> %s via {%s,%d} = Collision {%s,%d}\n",
            getXstateName(from), getXstateName(to),
            getXstateName(via), tms,
            getXstateName(transitions[from][to].via), transitions[from][to].tms);
      return;
   }
   else {
//      printf("%s -> %s via %s = OK\n", getXstateName(from), getXstateName(to), getXstateName(via));
      transitions[from][to].via = via;
      transitions[from][to].tms = tms;
   }
}

void printTransition(Xstate from, Xstate to) {
   bool first = true;
   Xstate current = from;
   do {
      Xstate last = current;
      current = transitions[current][to].via;
      if (current == Illegal) {
//         printf(" -> %s", getXstateName(to));
//         printf(" (Failed)\n");
         return;
      }
      if (first) {
         printf("%s", getXstateName(from));
         first = false;
      }
      if ((current == last) && (current != to)) {
         printf(" -> %s", getXstateName(current));
         printf(" (Failed - loop)\n");
         return;
      }
      if ((current == to)) {
         printf(" -> *%s*\n", getXstateName(current));
         return;
      }
      printf(" -> %s", getXstateName(current));
   } while (current != to);
   printf("\n");
}

void createTransitionTable() {

   // Fill table with 'Illegal'
   for(Xstate from=Reset; from<=IR_Update; ++from) {
      for(Xstate to=Reset; to<=IR_Update; ++to) {
         addTransition(from,to,Illegal, -1);
      }
   }

   // Add Transitions between stable states
   //=====================================
   addTransition(Reset,       Reset,       Reset,        1);

   addTransition(Reset,       Idle,        Idle,         0);

   addTransition(Reset,       DR_Pause,    Idle,         0);
   addTransition(Idle,        DR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   DR_Pause,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Pause,    DR_Exit1,     1);
   addTransition(DR_Exit1,    DR_Pause,    DR_Pause,     0);

   addTransition(Reset,       IR_Pause,    Idle,         0);
   addTransition(Idle,        IR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   IR_Pause,    IR_Select,    1);
   addTransition(IR_Select,   IR_Pause,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Pause,    IR_Exit1,     1);
   addTransition(IR_Exit1,    IR_Pause,    IR_Pause,     0);

   addTransition(Idle,        Reset,       DR_Select,    1);
   addTransition(DR_Select,   Reset,       IR_Select,    1);
   addTransition(IR_Select,   Reset,       Reset,        1);

   addTransition(Idle,        Idle,        Idle,         0);

   addTransition(Idle,        DR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   DR_Pause,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Pause,    DR_Exit1,     1);
   addTransition(DR_Exit1,    DR_Pause,    DR_Pause,     0);

   addTransition(Idle,        IR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   IR_Pause,    IR_Select,    1);
   addTransition(IR_Select,   IR_Pause,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Pause,    IR_Exit1,     1);
   addTransition(IR_Exit1,    IR_Pause,    IR_Pause,     0);

   addTransition(DR_Pause,    Reset,       DR_Exit2,     1);
   addTransition(DR_Exit2,    Reset,       DR_Update,    1);
   addTransition(DR_Update,   Reset,       DR_Select,    1);
   addTransition(DR_Select,   Reset,       IR_Select,    1);
   addTransition(IR_Select,   Reset,       Reset,        1);

   addTransition(DR_Pause,    Idle,        DR_Exit2,     1);
   addTransition(DR_Exit2,    Idle,        DR_Update,    1);
   addTransition(DR_Update,   Idle,        Idle,         0);

   addTransition(DR_Pause,    DR_Pause,    DR_Exit2,     1);
   addTransition(DR_Exit2,    DR_Pause,    DR_Update,    1);
   addTransition(DR_Update,   DR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   DR_Pause,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Pause,    DR_Exit1,     1);
   addTransition(DR_Exit1,    DR_Pause,    DR_Pause,     0);

   addTransition(DR_Pause,    IR_Pause,    DR_Exit2,     1);
   addTransition(DR_Exit2,    IR_Pause,    DR_Update,    1);
   addTransition(DR_Update,   IR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   IR_Pause,    IR_Select,    1);
   addTransition(IR_Select,   IR_Pause,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Pause,    IR_Exit1,     1);
   addTransition(IR_Exit1,    IR_Pause,    IR_Pause,     0);

   addTransition(IR_Pause,    Reset,       IR_Exit2,     1);
   addTransition(IR_Exit2,    Reset,       IR_Update,    1);
   addTransition(IR_Update,   Reset,       DR_Select,    1);
   addTransition(DR_Select,   Reset,       IR_Select,    1);
   addTransition(IR_Select,   Reset,       Reset,        1);

   addTransition(IR_Pause,    Idle,        IR_Exit2,     1);
   addTransition(IR_Exit2,    Idle,        IR_Update,    1);
   addTransition(IR_Update,   Idle,        Idle,         0);

   addTransition(IR_Pause,    DR_Pause,    IR_Exit2,     1);
   addTransition(IR_Exit2,    DR_Pause,    IR_Update,    1);
   addTransition(IR_Update,   DR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   DR_Pause,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Pause,    DR_Exit1,     1);
   addTransition(DR_Exit1,    DR_Pause,    DR_Pause,     0);

   addTransition(IR_Pause,    IR_Pause,    IR_Exit2,     1);
   addTransition(IR_Exit2,    IR_Pause,    IR_Update,    1);
   addTransition(IR_Update,   IR_Pause,    DR_Select,    1);
   addTransition(DR_Select,   IR_Pause,    IR_Select,    1);
   addTransition(IR_Select,   IR_Pause,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Pause,    IR_Exit1,     1);
   addTransition(IR_Exit1,    IR_Pause,    IR_Pause,     0);

   // Add transition between adjacent states (where they don't conflict with above)
   //===============================================================================

   addTransition(Reset,       Reset,       Reset,        1);
   addTransition(Reset,       Idle,        Idle,         0);

   addTransition(Idle,        Idle,        Idle,         0);
   addTransition(Idle,        DR_Select,   DR_Select,    1);
   addTransition(DR_Select,   DR_Capture,  DR_Capture,   0);
   addTransition(DR_Select,   IR_Select,   IR_Select,    1);
   addTransition(DR_Capture,  DR_Shift,    DR_Shift,     0);
   addTransition(DR_Capture,  DR_Exit1,    DR_Exit1,     1);
   addTransition(DR_Shift,    DR_Shift,    DR_Shift,     0);
   addTransition(DR_Shift,    DR_Exit1,    DR_Exit1,     1);
   addTransition(DR_Exit1,    DR_Pause,    DR_Pause,     0);
   addTransition(DR_Exit1,    DR_Update,   DR_Update,    1);
// addTransition(DR_Pause,    DR_Pause,    DR_Pause,     0);
   addTransition(DR_Pause,    DR_Exit2,    DR_Exit2,     1);
   addTransition(DR_Exit2,    DR_Shift,    DR_Shift,     0);
   addTransition(DR_Exit2,    DR_Update,   DR_Update,    1);
   addTransition(DR_Update,   Idle,        Idle,         0);
   addTransition(DR_Update,   DR_Select,   DR_Select,    1);

   addTransition(IR_Select,   IR_Capture,  IR_Capture,   0);
   addTransition(IR_Select,   Reset,       Reset,        1);
   addTransition(IR_Capture,  IR_Shift,    IR_Shift,     0);
   addTransition(IR_Capture,  IR_Exit1,    IR_Exit1,     1);
   addTransition(IR_Shift,    IR_Shift,    IR_Shift,     0);
   addTransition(IR_Shift,    IR_Exit1,    IR_Exit1,     1);
   addTransition(IR_Exit1,    IR_Pause,    IR_Pause,     0);
   addTransition(IR_Exit1,    IR_Update,   IR_Update,    1);
// addTransition(IR_Pause,    IR_Pause,    IR_Pause,     0);
   addTransition(IR_Pause,    IR_Exit2,    IR_Exit2,     1);
   addTransition(IR_Exit2,    IR_Shift,    IR_Shift,     0);
   addTransition(IR_Exit2,    IR_Update,   IR_Update,    1);
   addTransition(IR_Update,   Idle,        Idle,         0);
   addTransition(IR_Update,   DR_Select,   DR_Select,    1);

   // Add useful transition (where they don't conflict with above)
   //===============================================================================
   addTransition(Idle,        DR_Shift,    DR_Select,    1);
   addTransition(DR_Select,   DR_Shift,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Shift,    DR_Shift,     0);

   addTransition(DR_Shift,    Idle,        DR_Exit1,     1);
   addTransition(DR_Exit1,    Idle,        DR_Update,    1);
   addTransition(DR_Update,   Idle,        Idle,         0);

   addTransition(Idle,        IR_Shift,    DR_Select,    1);
   addTransition(DR_Select,   IR_Shift,    IR_Select,    1);
   addTransition(IR_Select,   IR_Shift,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Shift,    IR_Shift,     0);

   addTransition(IR_Shift,    Idle,        IR_Exit1,     1);
   addTransition(IR_Exit1,    Idle,        IR_Update,    1);
   addTransition(IR_Update,   Idle,        Idle,         0);

   addTransition(IR_Pause,    DR_Shift,    IR_Exit2,     1);
   addTransition(IR_Exit2,    DR_Shift,    IR_Update,    1);
   addTransition(IR_Update,   DR_Shift,    DR_Select,    1);
   addTransition(DR_Select,   DR_Shift,    DR_Capture,   0);
   addTransition(DR_Capture,  DR_Shift,    DR_Shift,     0);

   addTransition(DR_Pause,    IR_Shift,    DR_Exit2,     1);
   addTransition(DR_Exit2,    IR_Shift,    DR_Update,    1);
   addTransition(DR_Update,   IR_Shift,    DR_Select,    1);
   addTransition(DR_Select,   IR_Shift,    IR_Select,    1);
   addTransition(IR_Select,   IR_Shift,    IR_Capture,   0);
   addTransition(IR_Capture,  IR_Shift,    IR_Shift,     0);
}

void printFullTransitionTable(FILE *fp) {

   fprintf(fp,
         "struct Transition {\n"
         "   Xstate via;\n"
         "   int8_t tms;\n"
         "};\n\n"
   );
   fprintf(fp, "const Transition fullTapTransitions[16][16] = {\n");
   fprintf(fp, "   // -From-      To->");
   for(Xstate state=Reset; state<=IR_Update; ++state) {
      fprintf(fp, "%-18s", getXstateName(state));
   }
   fprintf(fp, "\n");

   // Print table
   for(Xstate from=Reset; from<=IR_Update; ++from) {
      fprintf(fp, "   /* %-10s */ {", getXstateName(from));
      for(Xstate to=Reset; to<=IR_Update; ++to) {
         fprintf(fp, "{%-11s %2d}, ", getXstateNameComma(transitions[from][to].via), transitions[from][to].tms);
         }
      fprintf(fp, "},\n");
   }
   fprintf(fp, "};\n\n");
}

void printTransitionTable(FILE *fp) {

   fprintf(fp,
         "/**\n"
         " * This is the TMS value to move from one state to another.\n"
         " * It incorporates:\n"
         " *  - Single transitions between directly connected states\n"
         " *  - Transition sequence between stable-states.\n"
         " *  - Useful transition sequences IDLE->SHIFT_DR, IDLE->SHIFT_IR\n"
         " *\n"
         " * 0 => TMS = 0; 1 => TMS = 1; -1 => illegal transition\n"
         " */\n"
         "const int8_t tapTransitions[16][16] = {\n"
      );
   fprintf(fp, "   // -From-     To->");
   for(Xstate state=Reset; state<=IR_Update; ++state) {
      fprintf(fp, "%-11s", getXstateName(state));
   }
   fprintf(fp, "\n");

   // Print table
   for(Xstate from=Reset; from<=IR_Update; ++from) {
      fprintf(fp, "   /* %-10s */ {", getXstateName(from));
      for(Xstate to=Reset; to<=IR_Update; ++to) {
         fprintf(fp, "%2d,        ", transitions[from][to].tms);
         }
      fprintf(fp, "},\n");
   }
   fprintf(fp, "};\n");
}

void printStateTable(FILE *fp) {

   fprintf(fp,
         "\n"
         "/**\n"
         " * State table for JTAG state machine\n"
         " * Indicates next-state for each current state and TMS value\n"
         " */\n"
         "const Xstate stateTable[16][2] = {\n"
      );
   fprintf(fp, "   // State          TMS=0       TMS=1\n");

   // Print table
   for(Xstate from=Reset; from<=IR_Update; ++from) {
      Xstate next0=Illegal, next1=Illegal;
      for(Xstate to=Reset; to<=IR_Update; ++to) {
         if (transitions[from][to].via != Illegal) {
            if (transitions[from][to].tms == 0) {
               if ((next0 != Illegal) && (next0 != transitions[from][to].via)) {
                  printf("Conflict in state table {%s,%d} -> %s [vs %s]\n",
                        getXstateName(from), transitions[from][to].tms, getXstateName(transitions[from][to].via), getXstateName(next0));
               }
               next0 = transitions[from][to].via;
            }
            else {
               if ((next1 != Illegal) && (next1 != transitions[from][to].via)) {
                  printf("Conflict in state table {%s,%d} -> %s [vs %s]\n",
                        getXstateName(from), transitions[from][to].tms, getXstateName(transitions[from][to].via), getXstateName(next1));
               }
               next1 = transitions[from][to].via;
            }
         }
      }
      fprintf(fp, "   /* %-10s */ {%-11s %-10s},\n", getXstateName(from), getXstateNameComma(next0), getXstateName(next1));
   }
   fprintf(fp, "};\n");
}

void printStableTransitions() {

   for (Xstate from=Reset; from<=IR_Update; from = static_cast<Xstate>(from + 1)) {
      for (Xstate to=Reset; to<=IR_Update;  to = static_cast<Xstate>(to + 1)) {
         printTransition(from,to);
      }
   }
}

int main() {
   createTransitionTable();
   FILE *fp = fopen("JtagTables.cpp", "wt");
   fprintf(fp, "#include \"JtagTables.h\" \n\n" );

   printFullTransitionTable(fp);
   printTransitionTable(fp);
   printStateTable(fp);

   fclose(fp);
//   printStableTransitions();
   return 0;
}
