#include <inttypes.h>
#include "config.h"
#include "midi.h"
#include "FR1XB.h"
#include "EA7.h"

// Set to 1 to transfer Sysex as is
uint8_t transferSysEx = 0;


/*
 * Channel = [1..16]
 * Return 1 to transmit as is, 0 to discard
 */

uint8_t onControlChange(uint8_t channel, uint8_t ctrl, uint8_t value) {
   if (ctrl == 0 || ctrl == 32) {
      return 0;
   }

   return 1;
}

#define NPATCHSET 4
#define NPATCH 2

static int8_t last_active_set = -1;

static int8_t active_patch[NPATCHSET] = 
   { -1, -1, -1, -1};
static uint8_t patches[NPATCHSET][NPATCH][3] =
   { { {64, 4, 21} // accordion fr
     , {50, 4, 65} // BlowAltoSib
     }
   , { {15, 5, 81} // Porta Lead
     , {16, 4, 48} // Strings
     }
   , { {0 , 4, 11} // Vibraphone
     , {1 , 4, 22} // Meloharm
     }
   , { {24, 4, 0 } // Piano+strings
     , {26, 4, 0 } // Piano+choir
     }
   };

void setPatchset(uint8_t set) {
   if (last_active_set != set) {
      if (active_patch[set] == -1) {
         active_patch[set] = 0;
      }
      last_active_set = set;
   }
   else {
      if (active_patch[set] == -1 || active_patch[set]+1 == NPATCH) {
         active_patch[set] = 0;
      } else {
         active_patch[set] += 1;
      }
   }
   setPatch( EA7_CHANNEL_UPPER
         , patches[set][active_patch[set]][0]
         , patches[set][active_patch[set]][1]
         , patches[set][active_patch[set]][2]
         );
}

#define MODE_UPPER 0
#define MODE_ORCH  1
#define MODE_ORGAN 2
uint8_t right_mode = MODE_UPPER;

uint8_t onProgramChange(uint8_t channel, uint8_t value) {
   switch (channel) {
      case FR1XB_CHANNEL_BASS:
         switch (value) {
            case FR1XB_REG_LEFT_1A:
            case FR1XB_REG_LEFT_1B:
               ea7_set_variation(3);
               return 0;
            case FR1XB_REG_LEFT_2A:
            case FR1XB_REG_LEFT_2B:
               ea7_set_variation(2);
               return 0;
            case FR1XB_REG_LEFT_3A:
            case FR1XB_REG_LEFT_3B:
            case FR1XB_REG_LEFT_3C:
               ea7_set_variation(1);
               return 0;
            default:
               return 0;
         }
         break;
      case FR1XB_CHANNEL_CHORD:
         return 0;
      case FR1XB_CHANNEL_NOTE:
         switch (value) {
            case FR1XB_REG_RIGHT_1A:
            case FR1XB_REG_RIGHT_1B:
            case FR1XB_REG_RIGHT_1C:
            case FR1XB_REG_RIGHT_1D:
               setPatchset(0);
               return 0;
            case FR1XB_REG_RIGHT_2A:
            case FR1XB_REG_RIGHT_2B:
            case FR1XB_REG_RIGHT_2C:
            case FR1XB_REG_RIGHT_2D:
               setPatchset(1);
               return 0;
            case FR1XB_REG_RIGHT_3A:
            case FR1XB_REG_RIGHT_3B:
            case FR1XB_REG_RIGHT_3C:
            case FR1XB_REG_RIGHT_3D:
               setPatchset(2);
               return 0;
            case FR1XB_REG_RIGHT_4A:
            case FR1XB_REG_RIGHT_4B:
               setPatchset(3);
               return 0;
            default:
               return 0;
         }
         break;
      case FR1XB_CHANNEL_ORCH:
         switch (value) {
            case FR1XB_REG_RIGHT_ORCH_1A:
            case FR1XB_REG_RIGHT_ORCH_1B:
            case FR1XB_REG_RIGHT_ORCH_1C:
            case FR1XB_REG_RIGHT_ORCH_1D:
            case FR1XB_REG_RIGHT_ORCH_2A:
            case FR1XB_REG_RIGHT_ORCH_2B:
            case FR1XB_REG_RIGHT_ORCH_2C:
            case FR1XB_REG_RIGHT_ORCH_2D:
            case FR1XB_REG_RIGHT_ORCH_3A:
            case FR1XB_REG_RIGHT_ORCH_3B:
            case FR1XB_REG_RIGHT_ORCH_3C:
            case FR1XB_REG_RIGHT_ORCH_3D:
            case FR1XB_REG_RIGHT_ORCH_4A:
            case FR1XB_REG_RIGHT_ORCH_4B:
            case FR1XB_REG_RIGHT_ORCH_4C:
            case FR1XB_REG_RIGHT_ORCH_4D:
               right_mode = MODE_ORCH;
               return 0;
            case FR1XB_LESLY_SLOW1:
            case FR1XB_LESLY_FAST1:
            case FR1XB_LESLY_SLOW2:
            case FR1XB_LESLY_FAST2:
            case FR1XB_LESLY_SLOW3:
            case FR1XB_LESLY_FAST3:
            case FR1XB_LESLY_SLOW4:
            case FR1XB_LESLY_FAST4:
               right_mode = MODE_ORGAN;
               return 0;
         }
   }
   return 0;
}

// uint8_t onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
//    return 1;
// }
// 

uint8_t onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
   if (channel != 4) {
      return 1;
   }
   else {
      switch (right_mode) {
         case MODE_UPPER: return 1;
         case MODE_ORCH:
            switch (note) {
            case 57: ea7_set_ending(1); return 0;
            case 60: ea7_set_ending(2); return 0;
            case 63: ea7_set_ending(3); return 0;
            case 66: ea7_set_ending(4); return 0;
            default: return 0;
            }
         case MODE_ORGAN:
            switch (note) {
            case 57: ea7_set_intro(1); return 0;
            case 60: ea7_set_intro(2); return 0;
            case 63: ea7_set_intro(3); return 0;
            case 66: ea7_set_intro(4); return 0;
            default: return 0;
            }
      }
   }
   return 1;
}

// 
// uint8_t onKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) {
//    return 1;
// }
// 
// uint8_t onChannelAfterTouch(uint8_t channel, uint8_t pressure) {
//    return 1;
// }
// 
// uint8_t onPitchBend(uint8_t channel, uint16_t bend) {
//    return 1;
// }
