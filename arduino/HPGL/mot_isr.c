#include <math.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "HPGL.h"
#include "mot_isr.h"
#include "stepgen.h"
#include "sio_isr.h"

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~(_BV(bit)))

uint8_t   rate;
uint8_t   speed_rate;
uint16_t  pen_pause = 0;
uint8_t   oldpen;
uint8_t   olderaser;
uint8_t   servo_high;
uint8_t   servo_val;
uint8_t   eraser_high;
uint8_t   eraser_val;
uint16_t  updown_pause;
uint16_t  s;
int32_t   cur_speed;
int32_t   next_speed;
uint16_t  servo_value;
uint16_t  eraser_value;
uint32_t  pos_x;
uint32_t  pos_y;

uint16_t  chalk_pos;
uint8_t   dochalk;

extern volatile uint8_t corr_a, corr_b;
extern void do_adc(void);

#ifdef USE_SENSOR_A
uint8_t ca;
#endif
#ifdef USE_SENSOR_B
uint8_t cb;
#endif

register  uint8_t mot_state asm("r2");
register  uint8_t cmd_state asm("r3");

#ifdef USE_CHALK
void get_chalk_pos(void)
{
  cbi(ADCSRA, ADSC);
  ADMUX &= 0xFE;
  sbi(ADCSRA, ADSC);
  while(ADCSRA & 0x40){};
  chalk_pos = ADCL;
  chalk_pos |= (ADCH << 8);
  ADMUX |= 0x01;
  sbi(ADCSRA, ADSC);
}

void change_chalk(void)
{
    uint32_t tout;

#ifndef INVERT_CHALK_BUTTON
    while(!(PINC & CHALK_BUTTON))
#else
    while(PINC & CHALK_BUTTON)
#endif
    {
        PORTB |= STATUS_LED;
        tout = 0x000FFFFF;
        PORTB &= ~STATUS_LED;
        tout = 0x000FFFFF;
    }
}
#endif

#ifdef USE_Z_SERVO
ISR(TIMER0_COMPA_vect)
{
    TCNT0 = 0;
    
#ifndef USE_CHALK
#ifdef USE_SENSOR_A
  if(servo_value == ca)
#else
  if(servo_value == servo_high)
#endif
#else
  if(servo_value == servo_high)
#endif

#ifndef SERVO_INV_POL
    PORTD &= ~PEN_SERVO;
#else
    PORTD |= PEN_SERVO;
#endif
    servo_value++;
    
#ifdef USE_SENSOR_B
  if(eraser_value == cb)
#else
  if(eraser_value == eraser_high)
#endif
#ifndef ERASER_INV_POL
    PORTD &= ~ERASER_SERVO;
#else
    PORTD |= ERASER_SERVO;
#endif
    eraser_value++;

  if(servo_value == 1140)
  {
#ifdef USE_CHALK
    if(servo_high != 0)
#endif
#ifndef SERVO_INV_POL
    PORTD |= PEN_SERVO;
#else
    PORTD &= ~PEN_SERVO;
#endif
    servo_value = 0;  
  }

  if(eraser_value == 1140)
  {
#ifndef ERASER_INV_POL
    PORTD |= ERASER_SERVO;
#else
    PORTD &= ~ERASER_SERVO;
#endif
    eraser_value = 0;  
  }
}
#endif

// Motor Control IRQ
ISR(TIMER1_OVF_vect)
{
  sei();
  if(dochalk)
  {
      goto MOTISR_END;
  }

  if(pen_pause)
  {
    pen_pause--;
    TCNT1 = 0x1000;
  }
  else
  {
    TCNT1 = s;

    STEP_X_OFF;
#ifdef USE_X2
    STEP_X2_OFF;
#endif
    STEP_Y_OFF;

    get_next_step();

    if(!(mot_state & 0xE0) && !(cmd_state & 0xDF))
    {
#ifdef CHECK_CHALK_WHILE_DRAWING
#ifdef USE_CHALK
      if(oldpen == 1)
      {
#ifndef INVERT_CHALK_IR
        if(!(PINC & CHALK_IR))
#else
        if(PINC & CHALK_IR)
#endif
        {
          dochalk = 1;
          pen_down();
        }
      }
#endif
#endif
      if(mot_state & DIR_X_CW)
        STEP_X_CW;
      else
        STEP_X_CCW; 

      if(mot_state & DIR_Y_CW)
        STEP_Y_CW;
      else
        STEP_Y_CCW; 

      if(mot_state & DO_STEP_X)
      {
        if(mot_state & DIR_X_CW)
          pos_x--;
        else
          pos_x++;

        if(pos_x < XMAX && pos_x > 0)
        {
          STEP_X_ON;
/*
#ifdef USE_X2
          STEP_X2_ON;
#endif
*/
        }
      }
#ifdef USE_X2
      if(cmd_state & DO_STEP_X2)
      {
          STEP_X2_ON;
      }
#endif

      if(mot_state & DO_STEP_Y)
      {
        if(mot_state & DIR_Y_CW)
          pos_y--;
        else
          pos_y++;

        if(pos_y < YMAX && pos_y > 0)
          STEP_Y_ON;
      }
      goto MOTISR_END;
    }

    if(cmd_state & SET_VELOCITY)
    {
      cmd_state &= 0x7F;
      if(mot_state || cmd_state)
      {
        next_speed = (cmd_state<<8 | mot_state);
        next_speed <<= 1;
      }
    }
    else if(cmd_state & VELOCITY_RATE)
    {
      if(mot_state == 0)
        speed_rate = 255;
      else
        speed_rate = 256 - mot_state;
    }
    else if(mot_state & MOTOR_CTRL)
    {
      set_motor(mot_state&0x01);
    }
    else if(cmd_state & Z_PRESS)
    {
      set_servo(mot_state);
    }
    else if(mot_state & Z_UP)
    {
      if(oldpen == 1)
      {
#ifndef USE_CHALK
        pen_pause =  updown_pause;
#else
        dochalk = 1;
#endif
        pen_up();
        oldpen = 0;
      }
    }
    else if(mot_state & Z_DOWN)
    {
      if(oldpen == 0)
      {
#ifndef USE_CHALK
          pen_pause =  updown_pause;
#else
          dochalk = 1;
#endif
        pen_down();
        oldpen = 1;
      }
    }
    else if(cmd_state & ERASER_PRESS)
    {
      set_eraser(mot_state);
    }
    else if(cmd_state & ERASER_UP)
    {
      if(olderaser == 1)
      {
        pen_pause =  updown_pause;
        eraser_up();
        olderaser = 0;
      }
    }
    else if(cmd_state & ERASER_DOWN)
    {
      if(olderaser == 0)
      {
        pen_pause =  updown_pause;
        eraser_down();
        olderaser = 1;
      }
    }
  }
MOTISR_END:
  mot_state = STEP_DONE;
  cmd_state = 0;
}

// fast integer square-root
int32_t isqrt(int32_t a)
{
  int32_t ret=0;
  int32_t s;
  int32_t ret_sq=-a-1;
  int32_t b;
  for(s=30; s>=0; s-=2)
  {
    ret+= ret;
    b=ret_sq + ((2*ret+1)<<s);
    if(b<0)
    {
      ret_sq=b;
      ret++;
    }
  }
  return ret;
}

static uint8_t isr2state;

ISR(TIMER2_OVF_vect)
{
  sei();
  rate++;
  if(rate == speed_rate)
  {
    if(s < next_speed)
    {
      cur_speed+= 0x1000;
      s = isqrt(cur_speed) << 5; // 32;

      if(s > next_speed)
        s = next_speed;
    }
    else if(s > next_speed)
    {
      cur_speed-= 0x1000;
      s = isqrt(cur_speed) << 5; // * 32;

      if(s < next_speed)
        s = next_speed;
    }
    rate = 0;
  }


#ifndef USE_CHALK
#ifdef USE_SENSOR_A
  if(oldpen == 1)
  {
#ifdef SERVO_REVERSE
    cli();
    ca = servo_high + corr_a;
    if(ca > PEN_MAX)
        ca = PEN_MAX;
  }
  else
      ca = PEN_MAX;
#else
    ca = servo_high - corr_a;
    if(ca < PEN_MIN)
        ca = PEN_MIN;
  }
  else
      ca = PEN_MIN;
#endif
  sei();
#endif
#else
  if(oldpen == 1)
  {
      get_chalk_pos();
  }
#endif


#ifdef USE_SENSOR_B
  if(olderaser == 1)
  {
#ifdef ERASER_REVERSE
    cli();
    cb = eraser_high + corr_b;
    if(cb > ERASER_MAX)
        cb = ERASER_MAX;
  }
  else
      cb = ERASER_MAX;
#else
    cb = eraser_high - corr_b;
    if(cb < ERASER_MIN)
        cb = ERASER_MIN;
  }
  else
      cb = ERASER_MIN;
#endif
  sei();
#endif
  if(!(isr2state & 0x02))
  {
    isr2state |= 0x02;
    do_adc();
    isr2state &= ~0x02;
  }
  
  if(!(isr2state & 0x01))
  {
    isr2state |= 0x01;
    gen_steps();
    isr2state &= ~0x01;
  }
}

void set_motor(uint8_t mstate)
{
  if(mstate)
    PORTB |= MOTOR_IO;
  else
    PORTB &= ~MOTOR_IO;
}

#ifdef USE_Z_SERVO
void set_servo(uint8_t pressure)
{
#ifndef USE_CHALK
  if(pressure <= (PEN_MAX-PEN_MIN))
#ifndef SERVO_REVERSE
    servo_val = PEN_MIN+pressure;
#else
    servo_val = PEN_MAX-pressure;
#endif
  else
    servo_val = PEN_MAX; 
  if(oldpen == 1)
    servo_high = servo_val;
#endif
}

void set_eraser(uint8_t pressure)
{
  if(pressure <= (ERASER_MAX-ERASER_MIN))
#ifndef ERASER_REVERSE
    eraser_val = ERASER_MIN+pressure;
#else
    eraser_val = ERASER_MAX-pressure;
#endif
  else
    eraser_val = ERASER_MAX; 
  if(olderaser == 1)
    eraser_high = eraser_val;
}

void pen_down(void)
{
#ifndef USE_CHALK
    servo_high = servo_val; 
#else
    uint16_t new_chalk_pos;
again:
#ifndef INVERT_CHALK_IR
    while(!(PINC & CHALK_IR))
#else
    while(PINC & CHALK_IR)
#endif
    {
#ifndef INVERT_CHALK_SERVO
        servo_high = CHALK_MIN;
#else
        servo_high = CHALK_MAX;
#endif
        get_chalk_pos();
#ifndef INVERT_CHALK_EMPTY
        if(chalk_pos < CHALK_EMPTY)
#else
        if(chalk_pos > CHALK_EMPTY)
#endif
        {
#ifndef INVERT_CHALK_SERVO
            servo_high = CHALK_MAX;
#else
            servo_high = CHALK_MIN;
#endif
#ifndef INVERT_CHALK_UP
            while(chalk_pos < CHALK_UP){get_chalk_pos();};
#else
            while(chalk_pos > CHALK_UP){get_chalk_pos();};
#endif
            servo_high = CHALK_STOP;

//            pen_up();
            change_chalk();
/*
#ifndef INVERT_CHALK_IR
            while(!(PINC & CHALK_IR))
#else
            while(PINC & CHALK_IR)
#endif
            {
#ifndef INVERT_CHALK_SERVO
                servo_high = CHALK_MIN;
#else
                servo_high = CHALK_MAX;
#endif
            }
*/
            goto again;
        }
    }

    servo_high = CHALK_STOP;

    get_chalk_pos();
    new_chalk_pos = chalk_pos;
#ifndef INVERT_CHALK_SERVO
    servo_high = CHALK_MIN;
#else
    servo_high = CHALK_MAX;
#endif

#ifndef INVERT_CHALK_PUSH
    new_chalk_pos -= CHALK_PUSH;
    while(chalk_pos > new_chalk_pos){get_chalk_pos();};
#else
    old_chalk_pos += CHALK_PUSH;
    while(chalk_pos < new_chalk_pos){get_chalk_pos();};
#endif
    servo_high = CHALK_STOP;
    
    dochalk = 0;

#endif
}

void pen_up(void)
{
#ifndef USE_CHALK
#ifndef SERVO_REVERSE
  servo_high = PEN_MIN;
#else
  servo_high = PEN_MAX;
#endif
#else
  uint16_t new_chalk_pos;
#ifndef INVERT_CHALK_SERVO
  servo_high = CHALK_MAX;
#else
  servo_high = CHALK_MIN;
#endif

#ifndef INVERT_CHALK_IR
  while(PINC & CHALK_IR)
#else
  while(!(PINC & CHALK_IR))
#endif
    get_chalk_pos();


#ifndef INVERT_CHALK_UP
  new_chalk_pos = chalk_pos + CHALK_PUSH_UP;
  while(chalk_pos < new_chalk_pos){get_chalk_pos();};
#else
  new_chalk_pos = chalk_pos - CHALK_PUSH_UP;
  while(chalk_pos > new_chalk_pos){get_chalk_pos();};
#endif

  servo_high = CHALK_STOP;
  dochalk = 0;

#endif
}

void eraser_up(void)
{
#ifndef ERASER_REVERSE
  eraser_high = ERASER_MIN;
#else
  eraser_high = ERASER_MAX;
#endif
}

void eraser_down(void)
{
  eraser_high = eraser_val; 
}
#else
#ifdef USE_Z_PWM
void set_servo(uint8_t pressure)
{
  if(pressure > 127)
    pressure = 127;

  if(pressure != 0)
    servo_val = (128-pressure)<<1;
  else
    servo_val = 255;

  if(oldpen == 1)
    pen_down();
}

void set_eraser(uint8_t pressure)
{
  if(pressure > 127)
    pressure = 127;

  if(pressure != 0)
    eraser_val = (128-pressure)<<1;
  else
    eraser_val = 255;

  if(olderaser == 1)
    eraser_down();
}

void pen_up(void)
{
#ifdef PWM_INVERT
  OCR0A = 0;
#else
  OCR0A = 255;
#endif
}

void eraser_up(void)
{
#ifdef ERASER_PWM_INVERT
  OCR0B = 0;
#else
  OCR0B = 255;
#endif
}

void pen_down(void)
{
#ifdef PWM_INVERT
  OCR0A = 255-servo_val;
#else
  OCR0A = servo_val;
#endif
}

void eraser_down(void)
{
#ifdef ERASER_PWM_INVERT
  OCR0B = 255-eraser_val;
#else
  OCR0B = eraser_val;
#endif
}
#endif
#endif


void stop_mot_isr(void)
{
  TIMSK1 &= ~(1<<TOIE1);
  TIMSK2 &= ~(1<<TOIE2);
}

void start_mot_isr(void)
{
  TIMSK1 |= (1<<TOIE1);
  TIMSK2 |= (1<<TOIE2);
}

void init_mot_isr(void)
{
#ifdef USE_Z_SERVO
  TCCR0A = 0x00;
  TCCR0B = 0x01;
  TIMSK0 = 0x02;
  TCNT0 = 0;
  OCR0A = 0xFF;
#endif

#ifdef USE_Z_PWM
  TCCR0A = (1<<WGM00) | (1<<WGM01);
  TCCR0B = 0x04;
  DDRD |= (ERASER_SERVO | PEN_SERVO);
  TCCR0A |= ((1<<COM0A1) | (1<<COM0A0) | (1<<COM0B1) | (1<<COM0B0));
  OCR0A = 255;
  OCR0B = 255;
  servo_val = 70;
  eraser_val = 70;
#endif

  TCCR1A = 0x00;
  TCCR1B = 0x01;
  TCNT1 = 0;

  dochalk = 0;
  mot_state = STEP_DONE;
  cur_speed = HOME_SPEED_START;

  s = HOME_SPEED_START;
  rate = 0x00;
  speed_rate = DEFAULT_SPEED_RATE;
  next_speed = HOME_SPEED_END;

  TCCR2A = 0x00;
  TCCR2B = 0x02;
  ASSR |= (0<<AS2);
  TCNT2 = 0;

  pos_x = 1000;
  pos_y = 1000;

  oldpen = 0;
  olderaser = 0;
  servo_value = 0;
  eraser_value = 0;
  
#ifdef USE_Z_PWM
  servo_high = DEFAULT_PRESSURE;
  eraser_high = DEFAULT_PRESSURE;
#endif
#ifdef USE_Z_SERVO
#ifdef SERVO_REVERSE
  servo_high = PEN_MAX;
  eraser_high = ERASER_MAX;
  servo_val = PEN_MIN;
  eraser_val = ERASER_MIN;
#else
  servo_high = PEN_MIN;
  eraser_high = ERASER_MIN;
  servo_val = PEN_MAX;
  eraser_val = ERASER_MAX;
#endif
#endif
  updown_pause = DEFAULT_UPDOWN_PAUSE;
  isr2state = 0;
}
