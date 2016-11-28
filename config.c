#include <inttypes.h>
#include "config.h"
#include "midi.h"
#include "FR1XB.h"
#include "EA7.h"

// Set to 1 to transfer Sysex as is
int transferSysEx = 0;

/* Control-change
 * Channel = [1..16]
 * Return 1 to transmit as is, 0 to discard
 */
int onControlChange(uint8_t channel, uint8_t ctrl, uint8_t value) {
   switch (channel) {
      case FR1XB_CHANNEL_BASS:
         return 0;
      case FR1XB_CHANNEL_CHORD:
         return 0;
   }
   return 1;
}

/* Program-change
 * Channel = [1..16]
 * Return 1 to transmit as is, 0 to discard
 */
int onProgramChange(uint8_t channel, uint8_t value) {
   switch (channel) {
      case FR1XB_CHANNEL_BASS:
         switch (value) {
            case FR1XB_REG_LEFT_1A:
            case FR1XB_REG_LEFT_1B:
               ea7_set_variation(1);
               return 0;
               break;
            case FR1XB_REG_LEFT_2A:
            case FR1XB_REG_LEFT_2B:
               ea7_set_variation(2);
               return 0;
               break;
            case FR1XB_REG_LEFT_3A:
            case FR1XB_REG_LEFT_3B:
            case FR1XB_REG_LEFT_3C:
               ea7_set_variation(3);
               return 0;
               break;
            default:
               return 0;
               break;
         }
         break;
      case FR1XB_CHANNEL_CHORD:
         return 0;
      default:
         break;
   }
   return 1;
}

int onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
   return 1;
}

int onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
   return 1;
}

int onKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) {
   return 1;
}

int onChannelAfterTouch(uint8_t channel, uint8_t pressure) {
   return 1;
}

int onPitchBend(uint8_t channel, uint16_t bend) {
   return 1;
}
