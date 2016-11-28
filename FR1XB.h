/* Roland FR1XB Accordeon */

#ifndef FR1XB_H
#define FR1XB_H

#define FR1XB_CHANNEL_NOTE  1
#define FR1XB_CHANNEL_BASS  2
#define FR1XB_CHANNEL_CHORD 3
#define FR1XB_CHANNEL_ORCH  4
#define FR1XB_CHANNEL_BASIC 13

// PC, left-hand registers (bass and chord channels)
#define FR1XB_REG_LEFT_1A 4
#define FR1XB_REG_LEFT_1B 2
#define FR1XB_REG_LEFT_2A 3
#define FR1XB_REG_LEFT_2B 5
#define FR1XB_REG_LEFT_3A 6
#define FR1XB_REG_LEFT_3B 1
#define FR1XB_REG_LEFT_3C 0

// PC, right-hand registers (note channel)
#define FR1XB_REG_RIGHT_1A 0
#define FR1XB_REG_RIGHT_1B 1
#define FR1XB_REG_RIGHT_1C 2
#define FR1XB_REG_RIGHT_1D 3
#define FR1XB_REG_RIGHT_2A 7
#define FR1XB_REG_RIGHT_2B 4
#define FR1XB_REG_RIGHT_2C 5
#define FR1XB_REG_RIGHT_2D 6
#define FR1XB_REG_RIGHT_3A 11
#define FR1XB_REG_RIGHT_3B 8
#define FR1XB_REG_RIGHT_3C 9
#define FR1XB_REG_RIGHT_3D 10
#define FR1XB_REG_RIGHT_4A 13
#define FR1XB_REG_RIGHT_4B 12

// PC, right-hand orchestra registers (orch channel)
#define FR1XB_REG_RIGHT_ORCH_1A 0
#define FR1XB_REG_RIGHT_ORCH_1B 1
#define FR1XB_REG_RIGHT_ORCH_1C 2
#define FR1XB_REG_RIGHT_ORCH_1D 3
#define FR1XB_REG_RIGHT_ORCH_2A 4
#define FR1XB_REG_RIGHT_ORCH_2B 5
#define FR1XB_REG_RIGHT_ORCH_2C 6
#define FR1XB_REG_RIGHT_ORCH_2D 7
#define FR1XB_REG_RIGHT_ORCH_3A 8
#define FR1XB_REG_RIGHT_ORCH_3B 9
#define FR1XB_REG_RIGHT_ORCH_3C 10
#define FR1XB_REG_RIGHT_ORCH_3D 11
#define FR1XB_REG_RIGHT_ORCH_4A 12
#define FR1XB_REG_RIGHT_ORCH_4B 13
#define FR1XB_REG_RIGHT_ORCH_4C 14
#define FR1XB_REG_RIGHT_ORCH_4D 15

// PC, organ lesly (orch channel)
#define FR1XB_LESLY_SLOW1 26
#define FR1XB_LESLY_SLOW2 27
#define FR1XB_LESLY_SLOW3 28
#define FR1XB_LESLY_SLOW4 29
#define FR1XB_LESLY_FAST1 36
#define FR1XB_LESLY_FAST2 37
#define FR1XB_LESLY_FAST3 38
#define FR1XB_LESLY_FAST4 39

// CC, accordion expression (note channel)
#define FR1XB_ACCORDION_EXPR 11

#endif
