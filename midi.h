#ifndef MIDI_H
#define MIDI_H

void sendControlChange(char channel, char ctrl, char value);
void sendProgramChange(char channel, char value);

#endif
