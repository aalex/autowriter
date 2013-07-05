#ifndef _PARSER_H_
#define _PARSER_H_


/*
// Standard HPGL Commands
#define FS      0x4653
#define PA	0x5041
#define PD	0x5044
#define PR	0x5052
#define PU	0x5055
#define SC	0x5343
#define SP      0x5350
#define VS	0x5653

// Custom Commands
#define ZQ      0x5A51    // status
*/

// Standard HPGL Commands
#define IN      (('I' << 8) | 'N')    // INit plotter
#define FS      (('F' << 8) | 'S')    // Force Select (Select Pen-Pressure)
#define PA      (('P' << 8) | 'A')    // Plot Absolute
#define PD	(('P' << 8) | 'D')    // Pen Down
#define PR	(('P' << 8) | 'R')    // Plot Relative
#define PU	(('P' << 8) | 'U')    // Pen Up
#define VS      (('V' << 8) | 'S')    // Velocity Select
#define EP      (('E' << 8) | 'P')    // Select Eraser-Pressure
#define ED	(('E' << 8) | 'D')    // Eraser Down
#define EU	(('E' << 8) | 'U')    // Eraser Up
#define MO	(('M' << 8) | 'O')    // Motor Control
#define XP	(('X' << 8) | 'P')    // pause-time for pen/eraser up/down
#define XS	(('X' << 8) | 'S')    // set the scaling-factor, it is param / 10000, so 10000 = 1:1
#define XO	(('X' << 8) | 'O')    // set the offset, offest point is scaled with the drawing scale
#define XR	(('X' << 8) | 'R')    // set the accelleration/decelleration rate
#define XX	(('X' << 8) | 'X')    // test

//Custom Commands
#define ZQ      (('Z' << 8) | 'Q')    // status

#define MIRROR_X    0x01
#define MIRROR_Y    0x02

void init_parser(void);
void parse(void);

#endif

