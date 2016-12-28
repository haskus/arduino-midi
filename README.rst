Haskus Arduino-MIDI
===================

Use an Arduino Uno microcontroller and the Olimex `SHIELD-MIDI`_ shield to
filter MIDI events.

.. _`SHIELD-MIDI`: https://www.olimex.com/Products/Duino/Shields/SHIELD-MIDI/

* Write your own `config.ino` file to react to MIDI messages in appropriate
  handlers (e.g., `onProgramChange`, `onControlChange`, `onNoteOn`, etc.).
* Use the MIDI interface described into `midi.h` to send MIDI mesages

The given `config.ino` file is configured to interconnect a Roland FR-1xb with a
Roland E-A7 keyboard arranger. See http://www.haskus.org/arduino/midi
