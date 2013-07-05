#ifndef _HOMING_H_
#define _HOMING_H_

#define TRIP_HOME_X    0x01
#define HOMED_X        0x02
#define TRIP_HOME_Y    0x04
#define HOMED_Y        0x08
#define IS_HOMED_FAST  0x10
#define TRIP_HOME_X2   0x20
#define HOMED_X2       0x40
#define IS_HOMED       0x80

#ifdef HOME_X_NEG
  #define IS_HOME_X   (PINB & HOME_X)
  #define NOT_HOME_X !(PINB & HOME_X)
#else
  #define IS_HOME_X  !(PINB & HOME_X)
  #define NOT_HOME_X  (PINB & HOME_X)
#endif

#ifdef HOME_X2_NEG
  #define IS_HOME_X2   (PINB & HOME_X2)
  #define NOT_HOME_X2 !(PINB & HOME_X2)
#else
  #define IS_HOME_X2  !(PINB & HOME_X2)
  #define NOT_HOME_X2  (PINB & HOME_X2)
#endif

#ifdef HOME_Y_NEG
  #define IS_HOME_Y   (PINB & HOME_Y)
  #define NOT_HOME_Y !(PINB & HOME_Y)
#else
  #define IS_HOME_Y  !(PINB & HOME_Y)
  #define NOT_HOME_Y  (PINB & HOME_Y)
#endif

void do_homing_fast(void);
void do_homing_slow(void);

#endif
