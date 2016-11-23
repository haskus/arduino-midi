void sendControlChange(byte channel, byte ctrl, byte value);
void sendProgramChange(byte channel, byte value);
void allumerRouge();
void eteindreRouge();
void allumerVert();
void eteindreVert();


/* Que faire lorsqu'on reçoit un Control Change?
 *
 * Pour le transmettre tel quel, renvoyer 1. Sinon renvoyer 0.
 */
int onControlChange(byte channel, byte ctrl, byte value) {
   // if (channel == 5 && ctrl == 10) {
   //    sendControlChange(4,10,value);
   //    sendProgramChange(4,5);
   //    return 0;
   // }
   return 1;
}

/* Que faire lorsqu'on reçoit un Program Change?
 *
 * Pour le transmettre tel quel, renvoyer 1. Sinon renvoyer 0.
 */
int onProgramChange(byte channel, byte value) {
   // if (channel == 5 && value == 10) {
   //    sendControlChange(4,10,value);
   //    sendProgramChange(4,5);
   //    return 0;
   // }
   return 1;
}


// ===============================================================================

// Configuration, exécutée une seule fois
void setup() {
   // Le protocol midi fonctionne à 31250 bits par seconde
   Serial.begin(31250);

   // On autorise l'utilisation des LEDs sur la carte MIDI
   pinMode(6,OUTPUT); // Rouge
   pinMode(7,OUTPUT); // Verte
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
byte readNext() {
   // on attend qu'un octet soit disponible
   while (!Serial.available());

   // on le lit et on le renvoie
   return Serial.read();
}

// Test si on a un Status Byte midi (MSB à 1)
int isStatusByte(byte b) {
   return b & 0x80 == 0x80;
}

// Octet lu
byte b;

// Ecrit b, puis copie les données (MSB=0) de l'entrée vers la sortie.
// Stocke le premier octot avec MSB=1 dans b.
void passthroughData() {
   do {
      Serial.write(b);
      b = readNext();
   } while (!isStatusByte(b));
}

// Envoie un Control Change
void sendControlChange(byte channel, byte ctrl, byte value) {
   Serial.write(0xB0 | channel);
   Serial.write(ctrl);
   Serial.write(value);
}

// Envoie un Program Change
void sendProgramChange(byte channel, byte value) {
   Serial.write(0xC0 | channel);
   Serial.write(value);
}


// Exécutée en boucle
void loop() {

   // D'abord on se synchronise sur le début du prochain message (si lorsqu'on
   // allume, on reçoit un morceau de message sans avoir eu le début, on
   // l'ignore). On ignore aussi le message "fin de SysEx" (0xF7) vu qu'on
   // n'aura pas transmis le début de toutes façons.
   do {
      b = readNext();
   } while (!isStatusByte(b) || b == 0xF7);

   // On est synchronisé, b contient le premier octet du message (status byte).
   // C'est là que les choses intéressantes commencent et qu'on peut choisir
   // quoi faire en fonction du message.
   do {

      // On transmet les System Messages comme on les reçoit
      if (b & 0xF0 == 0xF0) {
         if (b & 0xF == 0x00) { // System exclusive
            // Tant qu'on n'a pas reçu "fin de SysEx", on continue de
            // transmettre. Des System Realtime Messages peuvent être mélangés
            // avec les données du sysex mais vu qu'on ne veut pas les traiter
            // spécifiquement, on les transmet directement
            do {
               Serial.write(b);
               b = readNext();
            } while (b != 0xF7);
            Serial.write(b);
            b = readNext();
         } else {
            // On passe les octet de données tels quels
            passthroughData();
         }
      }

      else {
         // Numéro du canal midi
         byte channel = b & 0x0F;
         switch ((b >> 4) & 0x07) {
            case 3:  // Control change
               do {
                  byte ctrl  = readNext();
                  byte value = readNext();
                  if (onControlChange(channel,ctrl,value)) {
                     sendControlChange(channel,ctrl,value);
                  }
                  b = readNext();
               } while (0);
               break;
            case 4:  // Program change
               do {
                  byte value  = readNext();
                  if (onProgramChange(channel,value)) {
                     sendProgramChange(channel,value);
                  }
                  b = readNext();
               } while (0);
               break;
            //case 0:  // Note Off
            //case 1:  // Note On
            //case 2:  // Polyphonic key pressure (aftertouch)
            //case 5:  // Channel Pressure (after-touch)
            //case 6:  // Pitch bend change
            default:
               passthroughData();
               break;
         }
      }
   } while (1);
}

