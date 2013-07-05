#include <inttypes.h>
#include <avr/interrupt.h>
#include "HPGL.h"
#include "queue.h"
#include "parser.h"
#include "mot_isr.h"
#include "homing.h"
#include "stepgen.h"
#include "sio_isr.h"

#define IS_FINISHED   0x01
#define IS_REL        0x02
#define IS_LINE       0x04
#define IS_FASTLINE   0x08
#define X_BIGGER_Y    0x10

volatile uint8_t    num_steps;
uint8_t             next_step_write;
uint8_t             next_step_read;
volatile uint16_t   steps[MAX_STEPS];
uint8_t             step_mot_state;
uint8_t             step_state;
int32_t             dist;
queueEntry          qe;
int32_t             berror;
int8_t              offsetX, offsetY;
union longbytes     x;
union longbytes     y;
uint8_t             qt;
extern uint8_t      num_queue;
extern uint8_t      home_state;
extern uint8_t      mirror;

register uint16_t   next_smot_state asm("r2");

void init_stepgen(void)
{
  uint8_t cnt;
  for(cnt = 0; cnt < MAX_STEPS; cnt++)
  {
    steps[cnt] = 0; 
  }
  
  num_steps        = 0;
  next_step_write  = 0;
  next_step_read   = 0;
  step_mot_state   = 0;
  step_state       = IS_FINISHED;
}

void read_next_step(void)
{
  if(num_steps != 0)
  {
    next_smot_state = steps[next_step_read];
    steps[next_step_read] = 0;
    next_step_read++;
    next_step_read &= (MAX_STEPS-1);
    num_steps--;
#ifdef USE_X2
    if((next_smot_state & DO_STEP_X))
        next_smot_state |= (DO_STEP_X2 << 8);
#endif
  }
  else
    next_smot_state = STEP_DONE;
}

void write_next_step(uint16_t n_mot_state)
{
  cli();
  if(num_steps < MAX_STEPS)
  {
    sei();
    steps[next_step_write] = n_mot_state;
    next_step_write++;
    next_step_write &= (MAX_STEPS-1);
    cli();
    num_steps++;
  }
  sei();
}

void setup_bresenham(void)
{
  step_mot_state = 0;

  if(x.number == 0 && y.number == 0)
  {
    step_state = IS_FINISHED;
    return;
  }
  else if(x.number == 0)
  {
    step_state = IS_FASTLINE;
    step_mot_state = DO_STEP_Y;
    if (y.byte.high & 0x80)
    {
      step_mot_state |= DIR_Y_CW;
      dist = -y.number;
    }
    else
    {
      dist = y.number;
    }
    return;
  }
  else if(y.number == 0)
  {
    step_state = IS_FASTLINE;
    step_mot_state = DO_STEP_X;
    if (x.byte.high & 0x80)
    {
      step_mot_state |= DIR_X_CW;
      dist = -x.number;
    }
    else
    {
      dist = x.number;
    }
    return;
  }

  step_state = IS_LINE;

  if (x.byte.high & 0x80)
  {
    offsetX = -1;
    step_mot_state |= DIR_X_CW;
    x.number = -x.number;
  }
  else
  {
    offsetX = 1;
  }

  if (y.byte.high & 0x80)
  {
    offsetY = -1;
    step_mot_state |= DIR_Y_CW;
    y.number = -y.number;
  }
  else
  {
    offsetY = 1;
  }

  if (x.number > y.number)
  {
    step_state |= X_BIGGER_Y;
    step_mot_state |= DO_STEP_X;
    berror = x.number / 2;
    dist = x.number;
  }
  else
  {
    step_mot_state |= DO_STEP_Y;
    berror = y.number / 2; 
    dist = y.number;
  }
}

void bresenham(void)
{
  if (step_state & X_BIGGER_Y)
  {
    if (dist--)
    {
      berror = berror - y.number;
      step_mot_state &= ~DO_STEP_Y;
      if (berror < 0)
      {
        berror = berror + x.number;
        step_mot_state |= DO_STEP_Y;
      }
      write_next_step(step_mot_state);
    }
    else
    {
      step_state |= IS_FINISHED;
    }
  }
  else
  {
    if (dist--)
    {
      berror = berror - x.number;
      step_mot_state &= ~DO_STEP_X;
      if (berror < 0)
      {
        berror = berror + y.number;
        step_mot_state |= DO_STEP_X;
      }
      write_next_step(step_mot_state);
    }
    else
    {
      step_state |= IS_FINISHED;
    }
  }
}

void gen_steps(void)
{
  if(num_steps < MAX_STEPS)
  {
    if(step_state & IS_FINISHED)
    {
      if(num_queue)
      {
        read_queue_s();
        switch(qt)
        {
          case QUEUE_FS:
                                x.number &= 0x7F;
                                x.number += Z_PRESS<<8;
                                write_next_step(x.number);
                                break;
          case QUEUE_PA:
                                setup_bresenham();
                                break;
          case QUEUE_PD:
                                write_next_step(Z_DOWN);
                                setup_bresenham();
                                break;
          case QUEUE_PR:
                                setup_bresenham();
                                break;
          case QUEUE_PU:
                                write_next_step(Z_UP);
                                setup_bresenham();
                                break;
          case QUEUE_VS:
                                x.number &= 0x7FFF;
                                x.number += (SET_VELOCITY<<8);
                                write_next_step(x.number);
                                break;
          case QUEUE_EP:
                                x.number &= 0x7F;
                                x.number += ERASER_PRESS<<8;
                                write_next_step(x.number);
                                break;
          case QUEUE_ED:
                                write_next_step(ERASER_DOWN<<8);
                                setup_bresenham();
                                break;
          case QUEUE_EU:
                                write_next_step(ERASER_UP<<8);
                                setup_bresenham();
                                break;
          case QUEUE_MO:
                                x.number &= 0x01;
                                x.number += (MOTOR_CTRL);
                                write_next_step(x.number);
                                break;
          case QUEUE_XR:
                                x.number &= 0x00FF;
                                x.number += (VELOCITY_RATE<<8);
                                write_next_step(x.number);
                                break;
        }
      }
    }
    else if(!(step_state & IS_FINISHED))
    {
      if(step_state & IS_LINE)
      {
        bresenham();
      }
      else if(step_state & IS_FASTLINE)
      {
        if(dist--)
          write_next_step(step_mot_state);
        else
          step_state |= IS_FINISHED;
      }
    }
  }
}

void get_next_step(void)
{
  if(home_state & IS_HOMED)
  {
    if(num_steps)
    {
      read_next_step();
    }
  }
  else if(!(home_state & IS_HOMED_FAST))
  {
    do_homing_fast();
  }
  else if(!(home_state & IS_HOMED))
  {
    do_homing_slow();
  }
}

