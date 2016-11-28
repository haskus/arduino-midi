#ifndef CONFIG_H
#define CONFIG_H

#include <inttypes.h>

extern int transferSysEx;

int onControlChange(uint8_t channel, uint8_t ctrl, uint8_t value);
int onProgramChange(uint8_t channel, uint8_t value);
int onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
int onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
int onKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure);
int onChannelAfterTouch(uint8_t channel, uint8_t pressure);
int onPitchBend(uint8_t channel, uint16_t bend);

#endif
