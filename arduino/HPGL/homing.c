#include <avr/io.h>

#include "HPGL.h"
#include "mot_isr.h"
#include "homing.h"

volatile uint8_t home_state = 0x00;
uint8_t hcnt;
extern uint16_t s;
extern uint32_t next_speed;
register uint8_t next_mot_state asm("r2");
register uint8_t next_mot_type  asm("r3");

extern uint32_t pos_x, pos_y;

void do_homing_fast(void)
{
  next_mot_type = 0;
  PORTB &= ~STATUS_LED;
  pos_x = 1000;
  pos_y = 1000;

#ifdef USE_X2
  if((home_state & HOMED_X) && (home_state & HOMED_Y) && (home_state & HOMED_X2) && HOME_X && HOME_Y && HOME_X2)
#else
  if((home_state & HOMED_X) && (home_state & HOMED_Y) && HOME_X && HOME_Y )
#endif
  {
    home_state |= IS_HOMED_FAST;
    hcnt = 0;
  }
  else
  {
    next_mot_state &= ~DO_STEP_X;
#ifdef USE_X2
    next_mot_type  &= ~DO_STEP_X2;
#endif
    next_mot_state &= ~DO_STEP_Y;

    if(NOT_HOME_X)
    {
      next_mot_state |= DO_STEP_X;
      next_mot_state &= ~DIR_X_CW;
    }
    else
      home_state |= HOMED_X;

#ifdef USE_X2
    if(NOT_HOME_X2)
    {
      next_mot_type  |= DO_STEP_X2;
      next_mot_state &= ~DIR_X_CW;
    }
    else
      home_state |= HOMED_X2;
#endif
    if(NOT_HOME_Y)
    {
      next_mot_state |= DO_STEP_Y;
      next_mot_state &= ~DIR_Y_CW;
    }
    else
      home_state |= HOMED_Y;
  }
}

void do_homing_slow(void)
{
  next_mot_type = 0;
  next_mot_state |= DIR_X_CW;
  next_mot_state |= DIR_Y_CW;

  if(hcnt < 50)
  {
    next_speed = 0x4000;
    s = next_speed;

    hcnt++;
    next_mot_state |= DO_STEP_X;
#ifdef USE_X2
    next_mot_type  |= DO_STEP_X2;
#endif
    next_mot_state |= DO_STEP_Y; 
  }
  else if(hcnt < 240)
  {
    next_speed = 0x0200;
    s = next_speed;
    if(NOT_HOME_X)
    {
      next_mot_state |= DO_STEP_X;
      next_mot_state &= ~DIR_X_CW;
    }
#ifdef USE_X2
    if(NOT_HOME_X2)
    {
      next_mot_type  |= DO_STEP_X2;
      next_mot_state &= ~DIR_X_CW;
    }
#endif
    if(NOT_HOME_Y)
    {
      next_mot_state |= DO_STEP_Y;
      next_mot_state &= ~DIR_Y_CW;
    }
#ifdef USE_X2
    if(IS_HOME_X && IS_HOME_X2 && IS_HOME_Y)
#else
    if(IS_HOME_X && IS_HOME_Y)
#endif
      hcnt = 240;
  }
  else if(hcnt < 250)
  {
    next_speed = 0x0040;
    s = next_speed;
    hcnt++;
    next_mot_state |= DO_STEP_X;
#ifdef USE_X2
    next_mot_type  |= DO_STEP_X2;
#endif
    next_mot_state |= DIR_X_CW;
    next_mot_state |= DO_STEP_Y;
    next_mot_state |= DIR_Y_CW;
  }
  else
  {
    pos_x = XMAX;
    pos_y = YMAX;
    next_speed = DEFAULT_SPEED;
    s = next_speed;
    home_state |= IS_HOMED;
  }
}
