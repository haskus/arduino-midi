#include <avr/sleep.h>
#include <inttypes.h>
#include "midi.h"
#include "FR1XB.h"
#include "EA7.h"
#include "config.h"

void onRealtimeMessage(uint8_t msg);
void onSystemMessage(uint8_t msg, uint8_t d0, uint8_t d1);
void onChannelMessage(uint8_t msg, uint8_t channel, uint8_t d0, uint8_t d1);

// Configuration, exécutée une seule fois
void setup() {
   // Le protocol midi fonctionne à 31250 bits par seconde
   Serial.begin(31250);

   // On autorise l'utilisation des LEDs sur la carte MIDI
   pinMode(6,OUTPUT); // Rouge
   pinMode(7,OUTPUT); // Verte

   pinMode(13,OUTPUT); // LED de l'arduino
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

// Lit le prochain octet en entrée
uint8_t readNext() {
   int b;

   do {
     // on attend qu'un octet soit disponible
     while (Serial.available() <= 0) {
         set_sleep_mode(SLEEP_MODE_IDLE);
         sleep_enable();
         digitalWrite(13,LOW);
         sleep_mode();
         sleep_disable();
         digitalWrite(13,HIGH);
     }
     b = Serial.read();
   }
   while (b < 0);

   // on le lit et on le renvoie
   return b;
}

int isStatusByte(uint8_t b) {
   return b & 0x80 == 0x80;
}

int isSystemMessage(uint8_t b) {
   return b & 0xF0 == 0xF0;
}

int isSystemRealtimeMessage(uint8_t b) {
   return b & 0xF8 == 0xF8;
}

volatile uint8_t last_written_status = 0x0;

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

typedef enum {INIT, SYSEX, SYSMSG, CHANMSG} State;

// Exécutée en boucle
void loop() {

   State state = INIT;
   uint8_t status;
   uint8_t channel;
   uint8_t data[2];
   uint8_t reqdata = 0;
   uint8_t cdata = 0;

   do {
      uint8_t b = readNext();

      if (isSystemRealtimeMessage(b)) {
         onRealtimeMessage(b);
      }
      else if (isSystemMessage(b)) {
         status = b;
         state = SYSMSG;
         switch (status & 0x07) {
            case 0x00: // System exclusive
               state = SYSEX;
               // we transfer sysex directly if requested
               if (transferSysEx)
                  Serial.write(status);
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
               state = INIT;
               break;
            //case 0x08: // timing clock
            //case 0x09: // undefined
            //case 0x0A: // start
            //case 0x0B: // continue
            //case 0x0C: // stop
            //case 0x0D: // undefined
            //case 0x0E: // active sensing
            //case 0x0F: // reset
         }
      }
      else if (isStatusByte(b)) { // channel message
         status = b;
         state = CHANMSG;
         channel = status & 0x0F;
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
                  state = INIT;
               }
               break;
            case CHANMSG:
               data[cdata] = b;
               cdata += 1;
               if (cdata == reqdata) {
                  onChannelMessage((status >> 4) & 0x07,channel+1,data[0],data[1]);
                  // running status: we only reset the data counter
                  cdata = 0;
               }
               break;
         }
      }
   } while (1);

}

void onRealtimeMessage(uint8_t msg) {
   // we transmit realtime messages directly
   Serial.write(msg);
}

void onChannelMessage(uint8_t msg, uint8_t channel, uint8_t d0, uint8_t d1) {
   switch (msg) {
      case 0:  // Note Off
         if (onNoteOff(channel,d0,d1)) {
            sendNoteOff(channel,d0,d1);
         }
         break;
      case 1:  // Note On
         if (onNoteOn(channel,d0,d1)) {
            sendNoteOn(channel,d0,d1);
         }
         break;
      case 2:  // Polyphonic key pressure (aftertouch)
         if (onKeyAfterTouch(channel,d0,d1)) {
            sendKeyAfterTouch(channel,d0,d1);
         }
         break;
      case 3: // control change
         if (onControlChange(channel,d0,d1)) {
            sendControlChange(channel,d0,d1);
         }
         break;
      case 4: // program change
         if (onProgramChange(channel,d0)) {
            sendProgramChange(channel,d0);
         }
         break;
      case 5:  // Channel Pressure (after-touch)
         if (onChannelAfterTouch(channel,d0)) {
            sendChannelAfterTouch(channel,d0);
         }
         break;
      case 6:  // Pitch bend change
         if (onPitchBend(channel,d0 | (d1 << 8))) {
            sendPitchBend(channel,d0 | (d1 << 8));
         }
         break;
   }
}

void onSystemMessage(uint8_t msg, uint8_t d0, uint8_t d1) {
   switch (msg) {
      case 0xF1:  // MIDI Time Code Quarter Frame
      case 0xF3:  // Song select
         Serial.write(msg);
         Serial.write(d0);
         break;
      case 0xF2:  // Song position pointer
         Serial.write(msg);
         Serial.write(d0);
         Serial.write(d1);
         break;
      case 0xF4:  // Undefined
      case 0xF5:  // Undefined
      case 0xF6:  // Tune request
      case 0xF7:  // End of exclusive
         Serial.write(msg);
         break;
   }
}
