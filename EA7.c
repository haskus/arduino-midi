#include "EA7.h"
#include "midi.h"

// Define Varation [1-4]
void ea7_set_variation(uint8_t n) {
   sendControlChange(EA7_CHANNEL_STYLE,0,30);
   sendControlChange(EA7_CHANNEL_STYLE,32,29);
   uint8_t pc[] = { EA7_VARIATION1, EA7_VARIATION2
                  , EA7_VARIATION3, EA7_VARIATION4 };
   sendProgramChange(EA7_CHANNEL_STYLE, pc[n-1]);
}

