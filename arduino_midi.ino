#include <avr/sleep.h>
#include "midi.h"
#include "FR1XB.h"
#include "EA7.h"

void allumerRouge();
void eteindreRouge();
void allumerVert();
void eteindreVert();


/* Que faire lorsqu'on reçoit un Control Change?
 *
 * Pour le transmettre tel quel, renvoyer 1. Sinon renvoyer 0.
 * 
 * Channel = [1..16]
 */
int onControlChange(char channel, char ctrl, char value) {
   // if (channel == 5 && ctrl == 10) {
   //    sendControlChange(4,10,value);
   //    sendProgramChange(4,5);
   //    return 0;
   // }
   switch (channel) {
      case FR1XB_CHANNEL_BASS:
         return 0;
      case FR1XB_CHANNEL_CHORD:
         return 0;
   }
   return 1;
}

/* Que faire lorsqu'on reçoit un Program Change?
 *
 * Pour le transmettre tel quel, renvoyer 1. Sinon renvoyer 0.
 * 
 * Channel = [1..16]
 */
int onProgramChange(char channel, char value) {
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

// ===============================================================================

// Configuration, exécutée une seule fois
void setup() {
   // Le protocol midi fonctionne à 31250 bits par seconde
   Serial.begin(31250);

   sei();
   
   // On autorise l'utilisation des LEDs sur la carte MIDI
   pinMode(6,OUTPUT); // Rouge
   pinMode(7,OUTPUT); // Verte

   pinMode(13,OUTPUT); // LED de l'arduino
   digitalWrite(13,HIGH);
}

void allumerRouge() {
   digitalWrite(6,HIGH);
}
void eteindreRouge() {
   digitalWrite(6,LOW);
}
void allumerVert() {
   digitalWrite(7,HIGH);
}
void eteindreVert() {
   digitalWrite(7,LOW);
}

// Lit le prochain octet en entrée
char readNext() {
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

// Lit le prochain octet en entrée
char peekNext() {
   int b;

     // on attend qu'un octet soit disponible
     b = Serial.peek();
     while (b < 0) {
         set_sleep_mode(SLEEP_MODE_IDLE);
         sleep_enable();
         digitalWrite(13,LOW);
         sleep_mode();
         sleep_disable();
         digitalWrite(13,HIGH);
         b = Serial.peek();
     }

   // on le lit et on le renvoie
   return b;
}
// Test si on a un Status char midi (MSB à 1)
int isStatusByte(char b) {
   return (b & 0x80) == 0x80 ? 1 : 0;
}

volatile char last_read_status    = 0x0;
volatile char last_written_status = 0x0;

// Write status char (support running status)
void writeStatus(char status) {
   if (status != last_written_status) {
      Serial.write(status);
      last_written_status = status;
   }
}

void passthrough(char count) {
  for (char i=0; i < count; i++) {
    char b = readNext();
    Serial.write(b);
  }
}

// Ecrit le status si différent du dernier écrit. Puis lit et écrit count chars.
// Puis lit b.
void passthroughMessage(char status, char count) {
   writeStatus(status);
   passthrough(count);
}

/* Envoie un Control Change
 *  
 * Channel = [1..16]
 */
void sendControlChange(char channel, char ctrl, char value) {
   writeStatus(0xB0 | (channel-1));
   Serial.write(ctrl);
   Serial.write(value);
}

/* Envoie un Program Change
 *  
 * Channel = [1..16]
 */
void sendProgramChange(char channel, char value) {
   writeStatus(0xC0 | (channel-1));
   Serial.write(value);
}


// Exécutée en boucle
void loop() {

   unsigned char status;

   // D'abord on se synchronise sur le début du prochain message (si lorsqu'on
   // allume, on reçoit un morceau de message sans avoir eu le début, on
   // l'ignore). On ignore aussi le message "fin de SysEx" (0xF7) vu qu'on
   // n'aura pas transmis le début de toutes façons.
   do {
      status = readNext();
   } while (!isStatusByte(status));

   last_read_status = status;
   
   do {
      unsigned char b;

      if (!isStatusByte(status)) {
        sendControlChange(15,15,status);
         allumerRouge();
         while (1) {}
      }

      // On transmet les System Messages comme on les reçoit
      if ((status & 0xF0) == 0xF0) {
         allumerVert();
         switch (status & 0x0F) {
            case 0x00: // System exclusive

               // Tant qu'on n'a pas reçu "fin de SysEx", on continue de
               // transmettre. Des System Realtime Messages peuvent être mélangés
               // avec les données du sysex mais vu qu'on ne veut pas les traiter
               // spécifiquement, on les transmet directement
               //allumerRouge();
               writeStatus(status);
               do {
                  b = readNext();
                  Serial.write(b);
               } while (b != 0xF7);
               //eteindreRouge();
               break;
            case 0x01: // Time code quarter frame
            case 0x03: // Song select
               //allumerRouge();
               writeStatus(status);
               passthrough(1);
               //eteindreRouge();
               break;
            case 0x02: // Song position pointer
               //allumerRouge();
               writeStatus(status);
               passthrough(2);
               //eteindreRouge();
               break;
            case 0x04: // undefined
            case 0x05: // undefined
            case 0x06: // tune request
            case 0x07: // end of exclusive
            case 0x08: // timing clock
            case 0x09: // undefined
            case 0x0A: // start
            case 0x0B: // continue
            case 0x0C: // stop
            case 0x0D: // undefined
            case 0x0F: // reset
               //allumerRouge();
               Serial.write(status);
               //eteindreRouge();
               break;
            case 0x0E: // active sensing
               //allumerRouge();
               Serial.write(status);
               //eteindreRouge();
               break;
        }
        eteindreVert();
      }
      else {
         // Numéro du canal midi
         char channel = (status & 0x0F) + 1;
         switch ((status >> 4) & 0x07) {
            case 3:  // Control change
               do {
                  char ctrl  = readNext();
                  char value = readNext();
                  if (onControlChange(channel,ctrl,value)) {
                     sendControlChange(channel,ctrl,value);
                  }
               } while (0);
               break;
            case 4:  // Program change
               do {
                  char value  = readNext();
                  if (onProgramChange(channel,value)) {
                        sendProgramChange(channel,value);
                  }
               } while (0);
               break;
            case 0:  // Note Off
            case 1:  // Note On
            case 2:  // Polyphonic key pressure (aftertouch)
            case 6:  // Pitch bend change
               passthroughMessage(status,2);
               break;
               
            case 5:  // Channel Pressure (after-touch)
               passthroughMessage(status,1);
               break;
         }
      }

      status = peekNext();
      if (isStatusByte(status)) {
        last_read_status = status;
        Serial.read();
      }
      else {
        status = last_read_status;
      }
   } while (1);
}

