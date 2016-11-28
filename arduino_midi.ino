#include <avr/sleep.h>
#include <inttypes.h>
#include "midi.h"
#include "FR1XB.h"
#include "EA7.h"
#include "config.h"

uint8_t onRealtimeMessage(uint8_t status);
void onSystemMessage(uint8_t status, uint8_t d0, uint8_t d1);
void onChannelMessage(uint8_t status, uint8_t channel, uint8_t d0, uint8_t d1);

void setup() {
   // MIDI protocol bitrate
   Serial.begin(31250);

   // LEDs
   pinMode(6,OUTPUT); // Red
   pinMode(7,OUTPUT); // Green
   pinMode(13,OUTPUT); // Arduino's
   digitalWrite(13,HIGH);
}

void redLightOn() {
   digitalWrite(6,HIGH);
}
void redLightOff() {
   digitalWrite(6,LOW);
}
void greenLightOn() {
   digitalWrite(7,HIGH);
}
void greenLightOff() {
   digitalWrite(7,LOW);
}

// Read next byte, sleep in the meantime
uint8_t readNext() {
   while (Serial.available() <= 0) {
      set_sleep_mode(SLEEP_MODE_IDLE);
      sleep_enable();
      digitalWrite(13,LOW);
      sleep_mode();
      sleep_disable();
      digitalWrite(13,HIGH);
   }
   return Serial.read();
}

#define isStatusByte(b) ((b & 0x80) == 0x80)
#define isSystemMessage(b) ((b & 0xF0) == 0xF0)
#define isSystemRealtimeMessage(b) ((b & 0xF8) == 0xF8)

static uint8_t last_written_status = 0x0;

void resetStatus() {
   last_written_status = 0x0;
}

// Write status uint8_t (support running status)
void writeChannelStatus(uint8_t status) {
   if (status != last_written_status) {
      Serial.write(status);
      last_written_status = status;
   }
}

/* Send control-change
 *  
 * Channel = [1..16]
 */
void sendControlChange(uint8_t channel, uint8_t ctrl, uint8_t value) {
   writeChannelStatus(0xB0 | (channel-1));
   Serial.write(ctrl);
   Serial.write(value);
}

/* Envoie un Program Change
 *  
 * Channel = [1..16]
 */
void sendProgramChange(uint8_t channel, uint8_t value) {
   writeChannelStatus(0xC0 | (channel-1));
   Serial.write(value);
}

void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
   writeChannelStatus(0x80 | (channel-1));
   Serial.write(note);
   Serial.write(velocity);
}

void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
   writeChannelStatus(0x90 | (channel-1));
   Serial.write(note);
   Serial.write(velocity);
}

void sendKeyAfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) {
   writeChannelStatus(0xA0 | (channel-1));
   Serial.write(note);
   Serial.write(pressure);
}

void sendChannelAfterTouch(uint8_t channel, uint8_t pressure) {
   writeChannelStatus(0xD0 | (channel-1));
   Serial.write(pressure);
}

void sendPitchBend(uint8_t channel, uint16_t bend) {
   writeChannelStatus(0xE0 | (channel-1));
   Serial.write(bend & 0xFF);
   Serial.write(bend >> 8);
}

typedef enum {INIT, RUNNING, SYSEX, SYSMSG, CHANMSG} State;

// Exécutée en boucle
void loop() {

   State state = INIT;
   uint8_t status;
   uint8_t last_channel_status;
   uint8_t channel;
   uint8_t data[2];
   uint8_t reqdata = 0;
   uint8_t cdata = 0;

   do {
      uint8_t b = readNext();

      if (isStatusByte(b)) {
         if (isSystemMessage(b)) {
            status = b;
            state = SYSMSG;
            switch (status & 0x0F) {
               case 0x00: // System exclusive
                  state = SYSEX;
                  // we transfer sysex directly if requested
                  if (transferSysEx) {
                     resetStatus();
                     Serial.write(status);
                  }
                  break;
               case 0x01: // Time code quarter frame
               case 0x03: // Song select
                  reqdata = 1;
                  break;
               case 0x02: // Song position pointer
                  reqdata = 2;
                  break;
               case 0x04: // undefined
               case 0x05: // undefined
               case 0x06: // tune request
               case 0x07: // end of exclusive
                  onSystemMessage(status,data[0],data[1]);
                  state = RUNNING;
                  cdata = 0;
                  break;
               case 0x08: // timing clock
               case 0x09: // undefined
               case 0x0A: // start
               case 0x0B: // continue
               case 0x0C: // stop
               case 0x0D: // undefined
               case 0x0E: // active sensing
               case 0x0F: // reset
                  if (onRealtimeMessage(status)) {
                     sendRealtimeMessage(status);
                  }
                  state = RUNNING;
                  break;
            }
         }
         else { // channel message
            status = b;
            last_channel_status = b;
            state = CHANMSG;
            channel = status & 0x0F;
            cdata = 0;
            switch ((status >> 4) & 0x07) {
               case 0:  // Note Off
               case 1:  // Note On
               case 2:  // Polyphonic key pressure (aftertouch)
               case 3:  // Control change
               case 6:  // Pitch bend change
                  reqdata = 2;
                  break;
               case 4:  // Program change
               case 5:  // Channel Pressure (after-touch)
                  reqdata = 1;
                  break;
            }
         }
      }
      else { // data
         switch (state) {
            case INIT:
               break;
            case SYSEX:
               if (transferSysEx)
                  Serial.write(b);
               break;
            case SYSMSG:
               data[cdata] = b;
               cdata += 1;
               if (cdata == reqdata) {
                  onSystemMessage(status,data[0],data[1]);
                  state = RUNNING;
                  cdata = 0;
               }
               break;
            case RUNNING:
               status = last_channel_status;
            case CHANMSG:
               data[cdata] = b;
               cdata += 1;
               if (cdata == reqdata) {
                  onChannelMessage(status,channel+1,data[0],data[1]);
                  // running status: we only reset the data counter
                  cdata = 0;
               }
               break;
         }
      }
   } while (1);

}

uint8_t onRealtimeMessage(uint8_t status) {
   // we transmit realtime messages directly
   return 1;
}

void sendRealtimeMessage(uint8_t status) {
   Serial.write(status);
   resetStatus();
}

void onChannelMessage(uint8_t status, uint8_t channel, uint8_t d0, uint8_t d1) {
   switch ((status >> 4) & 0x07) {
      case 0:  // Note Off
         if (onNoteOff == 0 || onNoteOff(channel,d0,d1) != 0) {
            sendNoteOff(channel,d0,d1);
         }
         break;
      case 1:  // Note On
         if (onNoteOn == 0 || onNoteOn(channel,d0,d1) != 0) {
            sendNoteOn(channel,d0,d1);
         }
         break;
      case 2:  // Polyphonic key pressure (aftertouch)
         if (onKeyAfterTouch == 0 || onKeyAfterTouch(channel,d0,d1)) {
            sendKeyAfterTouch(channel,d0,d1);
         }
         break;
      case 3: // control change
         if (onControlChange == 0 || onControlChange(channel,d0,d1)) {
            sendControlChange(channel,d0,d1);
         }
         break;
      case 4: // program change
         if (onProgramChange == 0 || onProgramChange(channel,d0)) {
            sendProgramChange(channel,d0);
         }
         break;
      case 5:  // Channel Pressure (after-touch)
         if (onChannelAfterTouch == 0 || onChannelAfterTouch(channel,d0)) {
            sendChannelAfterTouch(channel,d0);
         }
         break;
      case 6:  // Pitch bend change
         if (onPitchBend == 0 || onPitchBend(channel,d0 | (d1 << 8))) {
            sendPitchBend(channel,d0 | (d1 << 8));
         }
         break;
   }
}

void onSystemMessage(uint8_t status, uint8_t d0, uint8_t d1) {
   switch (status) {
      case 0xF1:  // MIDI Time Code Quarter Frame
      case 0xF3:  // Song select
         Serial.write(status);
         Serial.write(d0);
         break;
      case 0xF2:  // Song position pointer
         Serial.write(status);
         Serial.write(d0);
         Serial.write(d1);
         break;
      case 0xF4:  // Undefined
      case 0xF5:  // Undefined
      case 0xF6:  // Tune request
      case 0xF7:  // End of exclusive
         Serial.write(status);
         break;
   }
   resetStatus();
}

void setPatch(uint8_t channel, uint8_t bank0, uint8_t bank1, uint8_t patch) {
   sendControlChange(channel, 0, bank0);
   sendControlChange(channel, 32, bank1);
   sendProgramChange(channel, patch);
}

void midiStart() {
   sendRealtimeMessage(0x0A);
};

void midiContinue() {
   sendRealtimeMessage(0x0B);
};

void midiStop() {
   sendRealtimeMessage(0x0C);
};
