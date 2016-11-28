#include "EA7.h"
#include "midi.h"

/* Variations [1-4]:
 * PC [0,1,8,9]
 */
void ea7_set_variation(uint8_t n) {
   uint8_t pc[] = { 0, 1, 8, 9};
   sendProgramChange(EA7_CHANNEL_STYLE, pc[n-1]);
}

/* Endings [1-4]:
 * PC [74,75,72,73]
 */
void ea7_set_ending(uint8_t n) {
   uint8_t pc[] = { 74, 75, 72, 73};
   sendProgramChange(EA7_CHANNEL_STYLE, pc[n-1]);
}

/* Intros [1-4]:
 * PC [66,67,64,65]
 */
void ea7_set_intro(uint8_t n) {
   uint8_t pc[] = { 66, 67, 64, 65 };
   sendProgramChange(EA7_CHANNEL_STYLE, pc[n-1]);
}

