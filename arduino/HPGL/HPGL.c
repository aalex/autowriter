#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
  
#include "HPGL.h"
#include "sio_isr.h"
#include "mot_isr.h"
#include "homing.h"
#include "parser.h"
#include "queue.h"
#include "stepgen.h"

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

extern volatile uint8_t home_state;

uint16_t adc_val1, adc_val2;
uint8_t adc_cnt, acc_cnt;
volatile uint8_t corr_a, corr_b;
uint16_t adc1, adc2;
uint8_t oldbuttons, buttons, testbutton, testoldbutton;
uint16_t buttoncnt;

void init_plotter(void)
{
  stop_mot_isr();
  init_mot_isr();
  init_queue();
  init_stepgen();
  init_parser();
  home_state = 0x00;
  start_mot_isr();
  sei();
/*
WAIT_HOME:
  if(!(home_state & IS_HOMED))
    goto WAIT_HOME;
*/
  PORTB |= STATUS_LED;
}

void setup(void)                    // run once, when the sketch starts
{
  DDRB = (STATUS_LED | MOTOR_IO | STEP_X2);
  DDRD = (DIR_X | DIR_Y | STEP_X | STEP_Y | PEN_SERVO);

  PORTD = 0x00;
  PORTB = STATUS_LED;

  DDRC = 0x00;
  PORTC = 0x3C;

  ADMUX = 0x40;
  DIDR0 = 0x03;
  ADCSRA = 0x87;
  adc_val1 = 0;
  adc_val2 = 0;
  adc_cnt = 0;
  adc1 = 0;
  adc2 = 0;
  corr_a = 0;
  corr_b = 0;
  sbi(ADCSRA, ADSC);
  buttons = 0x00;
  oldbuttons = 0x00;
  buttoncnt = 0;
  sio_init();
}

void do_adc(void)
{
#ifndef USE_CHALK
  static unsigned char xa;
#endif
  static unsigned char xb;
  if(!(ADCSRA & 0x40))
  {
    if(ADMUX & 0x01)
    {
#ifdef USE_SENSOR_B
      adc_val2 = ADCL;
      adc_val2 |= (ADCH << 8);
      adc2 += adc_val2;
      adc_cnt++;
      if(adc_cnt == SENSOR_OVSAMPLE)
      {
        adc2 /= SENSOR_OVSAMPLE;
        if(adc2 > SENSOR_B_MAX)
          adc2 = SENSOR_B_MAX;
        else if(adc2 < SENSOR_B_MIN)
          adc2 = SENSOR_B_MIN;
      
        adc2 -= SENSOR_B_MIN;
#ifdef INVERT_SENSOR_B
        adc2 = (SENSOR_B_MAX-SENSOR_B_MIN) - adc2;
#endif
        xb = adc2 / SENSOR_B_DIV;
        corr_b = xb;
        adc2 = 0;
        adc_cnt = 0;
      }
#endif
      ADMUX &= 0xFE;
    }
    else
    {
#ifndef USE_CHALK
#ifdef USE_SENSOR_A
      adc_val1 = ADCL;
      adc_val1 |= (ADCH << 8);
      adc1 += adc_val1;
      if(adc_cnt == (SENSOR_OVSAMPLE-1))
      {
        adc1 /= SENSOR_OVSAMPLE;
        if(adc1 > SENSOR_A_MAX)
          adc1 = SENSOR_A_MAX;
        else if(adc1 < SENSOR_A_MIN)
          adc1 = SENSOR_A_MIN;
        
        adc1 -= SENSOR_A_MIN;
#ifdef INVERT_SENSOR_A
        adc1 = (SENSOR_A_MAX-SENSOR_A_MIN) - adc1;
#endif
        xa = adc1 / SENSOR_A_DIV;
        corr_a = xa;
        adc1 = 0;
      }
#endif
#endif
      ADMUX |= 0x01;
    }
    sbi(ADCSRA, ADSC);
  }
}

void check_buttons(void)
{
  if(buttoncnt != 500)
  {
    buttoncnt++;
  }
  else
  {
    buttons = PINC;
    buttons &= 0x3C;
    if(buttons != oldbuttons)
    {
        testbutton = buttons;
        testbutton &= 0x04;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x04;
        if(testbutton != testoldbutton)
        {
        output('A');
        if(testbutton == 0)
            output('1');
        else
            output('0');
        output('\n');
        }
        
        testbutton = buttons;
        testbutton &= 0x08;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x08;
        if(testbutton != testoldbutton)
        {
        output('B');
        if(testbutton == 0)
            output('1');
        else
            output('0');
        output('\n');
        }
#ifndef USE_CHALK
        testbutton = buttons;
        testbutton &= 0x10;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x10;
        if(testbutton != testoldbutton)
        {
        output('C');
        if(testbutton == 0)
            output('1');
        else
            output('0');
        output('\n');
        }
    
        testbutton = buttons;
        testbutton &= 0x20;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x20;
        if(testbutton != testoldbutton)
        {
        output('D');
        if(testbutton == 0)
            output('1');
        else
            output('0');
        output('\n');
        }
#else
        testbutton = buttons;
        testbutton &= 0x10;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x10;
        if(testbutton != testoldbutton)
        {
            output('C');
            if(testbutton == 0)
                output('0');
            else
                output('1');
            output('\n');
        }
    
        testbutton = buttons;
        testbutton &= 0x20;
        testoldbutton = oldbuttons;
        testoldbutton &= 0x20;
        if(testbutton != testoldbutton)
        {
            output('D');
            if(testbutton == 0)
                output('0');
            else
                output('1');
            output('\n');
        }
#endif
        oldbuttons = buttons;
    }
    buttoncnt = 0;
  }
}

#ifdef USE_ARDUINO
void loop(void)
{
#else
int main(void) 
{
  setup();
#endif
  init_plotter();

MAIN_LOOP:
  check_buttons();
  parse();
  goto MAIN_LOOP;
}
