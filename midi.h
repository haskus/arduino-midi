#ifndef MIDI_H
#define MIDI_H

#include <inttypes.h>

void sendControlChange(uint8_t channel, uint8_t ctrl, uint8_t value);
void sendProgramChange(uint8_t channel, uint8_t value);
void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void sendKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure);
void sendChannelAfterTouch(uint8_t channel, uint8_t pressure);
void sendPitchBend(uint8_t channel, uint16_t bend);

void setPatch(uint8_t channel, uint8_t bank0, uint8_t bank1, uint8_t patch);

void redLightOn();
void redLightOff();
void greenLightOn();
void greenLightOff();



#endif
