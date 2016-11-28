/* Roland FR1xb Accordeon */

#ifndef FR1XB_H
#define FR1XB_H

#define FR1XB_CHANNEL_NOTE  1
#define FR1XB_CHANNEL_BASS  2
#define FR1XB_CHANNEL_CHORD 3
#define FR1XB_CHANNEL_BASIC 13

// PC, left-hand registers (bass and chord channels)
#define FR1XB_REG_LEFT_1A 4
#define FR1XB_REG_LEFT_1B 2
#define FR1XB_REG_LEFT_2A 3
#define FR1XB_REG_LEFT_2B 5
#define FR1XB_REG_LEFT_3A 6
#define FR1XB_REG_LEFT_3B 1
#define FR1XB_REG_LEFT_3C 0

/*
Soufflet
--------
Channel 1:
   CC11 + valeure -> soufflet

Main droite:
------------
Registre 1a
   CC0
   CC32
   PC0 (1b -> 1, 1c -> 2, 1d -> 3)
   CC7
   CC10
   CC91
   CC93
   CC100
   CC101
   CC6
   CC38
   CC100
   CC101

Reg   PC
1a    0
1b    1
1c    2
1d    3
2a    7
2b    4
2c    5
2d    6
3a    11
3b    8
3c    9
3d    10
4a    13
4b    12

Lesly orgue: PC
Rapide: 36, 37, 38, 39
Lent  : 26, 27, 28, 29

Registres main gauche:
----------------------
CC0 CC32 +
   Reg   PC
   1a    4
   1b    2
   2a    3
   2b    5
   3a    6
   3b    1
   3c    0
+
   CC7
   CC10
   CC91
   CC93

sur les canaux basse et accords main gauche

Registres orchestra main droite:
--------------------------------
Reg   PC
1a    0
1b    1
1c    2
1d    3
2a    4
2b    5
2c    6
2d    7
3a    8
3b    9
3c    10
3d    11
4a    12
4b    13
4c    14
4d    15
*/

#endif
