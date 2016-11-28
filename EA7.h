/* Roland E-A7 */

#include "midi.h"

#define EA7_CHANNEL_STYLE 10

/* Variations:
 * CC 0  30
 * CC 32 29
 * PC EA7_VARIATIONn
 */
#define EA7_VARIATION1 0
#define EA7_VARIATION2 1
#define EA7_VARIATION3 8
#define EA7_VARIATION4 9

// Define Varation [1-4]
void ea7_set_variation(uint8_t n) {
   sendControlChange(EA7_CHANNEL_STYLE,0,30);
   sendControlChange(EA7_CHANNEL_STYLE,32,29);
   uint8_t pc[] = { EA7_VARIATION1, EA7_VARIATION2
                  , EA7_VARIATION3, EA7_VARIATION4 };
   sendProgramChange(EA7_CHANNEL_STYLE, pc[n-1]);
}

/*
Canal 10

Ending:
   CC 0  30
   CC 32 29
   + PC  74  -- ending 1
     PC  75  -- ending 2
     PC  72  -- ending 3
     PC  73  -- ending 4

Intro:
   activation variation +
   CC 0  30
   CC 32 29
   + PC  66  -- intro 1
     PC  67  -- intro 2
     PC  64  -- intro 3
     PC  65  -- intro 4

Track mute:
   Sur la piste style/drums
   Pitch bend 0
   Pitch bend -59
   CC 10 64
   CC 11 94
   CC 91 85
   CC 93 00

Sons:
   Accordeon fr:
      CC 0  64
      CC 32 4
      PC 21
   BlowAltoSib
      CC 0  50
      CC 32 4
      PC 65
   PortaLead
      CC 0  15
      CC 32 5
      PC 81
   Strings
      CC 0  16
      CC 32 4
      PC 48
   Vibraphone
      CC 0  0
      CC 32 4
      PC 11
   Meloharm
      CC 0  1
      CC 32 4
      PC 22
   Piano+string
      CC 0  24
      CC 32 4
      PC 0
   Piano+choir
      CC 0  26
      CC 32 4
      PC 0
*/
