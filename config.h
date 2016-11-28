#ifndef MIDI_CONFIG_H
#define MIDI_CONFIG_H

#include <inttypes.h>

extern uint8_t transferSysEx;

uint8_t onControlChange(uint8_t channel, uint8_t ctrl, uint8_t value)    __attribute__((weak));
uint8_t onProgramChange(uint8_t channel, uint8_t value)                  __attribute__((weak));
uint8_t onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)       __attribute__((weak));
uint8_t onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)        __attribute__((weak));
uint8_t onKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) __attribute__((weak));
uint8_t onChannelAfterTouch(uint8_t channel, uint8_t pressure)           __attribute__((weak));
uint8_t onPitchBend(uint8_t channel, uint16_t bend)                      __attribute__((weak));

#endif
