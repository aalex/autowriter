#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "HPGL.h"
#include "sio_isr.h"
#include "mot_isr.h"
#include "parser.h"
#include "queue.h"
#include "stepgen.h"

uint16_t  CMD;
uint8_t   num_param;
int32_t   parser_pos_x = XMAX;
int32_t   parser_pos_y = YMAX;
int32_t   p;
int32_t   p1;
int32_t   p2;
uint8_t   is_neg;
uint8_t   c;
uint8_t   pdone = 1;
uint8_t   pt = 0;
uint8_t   queue_rel = 0;
uint8_t   mirror;

extern volatile uint8_t     num_steps;
float scale_x;
float scale_y;
uint32_t offset_x;
uint32_t offset_y;
extern uint8_t  num_queue;
extern uint16_t   updown_pause;
extern uint8_t   speed_rate;

extern unsigned short adc1, adc2;

extern uint16_t  chalk_pos;
extern uint8_t   servo_high;

void init_parser(void)
{
  CMD = 0x00;
  num_param = 0;
  pdone = 1;
  parser_pos_x = XMAX;
  parser_pos_y = YMAX;
  pt = 0;
  queue_rel = 0;
  scale_x = XSCALE;
  scale_y = YSCALE;
  offset_x = 0;
  offset_y = 0;
}

void mul_ten_long(int32_t *l)
{
   static int32_t t;

   t = *l << 1;
   *l <<= 3;
   *l += t;
}

void mul_ten_char(uint8_t *c)
{
   static uint8_t t;

   t = *c << 1;
   *c <<= 3;
   *c += t;
}

static void cmd_done(void)
{
  CMD = 0x0000;
  pdone = 1;
}

void parse_single(void)
{
  if(pdone == 2)
  {
    goto DONE; 
  } else if(pdone == 1)
  {
    c = 0;
    p1 = 0;
    p2 = 0;
    num_param = 0;
    pdone = 0;
  }
  else if(isinput())
  {
    c = input();
    if(c == ';' || c == '\n')
      goto DONE;
    if(c >= '0' && c <= '9')
    {
      mul_ten_long(&p1);
      p1 += (c-'0');
      num_param |= 0x80;
    }
  }
  return;
DONE:
  if(num_param)
  {
    if(pt == QUEUE_XP)
    {
      updown_pause = p1;
      cmd_done();
    }
    else if(num_queue < MAX_QUEUE)
    {
      add_queue();
      cmd_done();
    }
    else
      pdone = 2;
  }
  else
    cmd_done();
}

void aq(void)
{
  if(num_param == 2)
  {
    add_queue();
    parser_pos_x += p1;
    parser_pos_y += p2;
  }
  else
  {
    p1 = 0;
    p2 = 0;
    add_queue();
  }
  num_param = 0;
}


void parse_multi(void)
{
  if(pdone == 3)
  {
    goto DONE; 
  }
  else if(pdone == 2)
  {
    if(num_queue < MAX_QUEUE)
    {
      aq();
      pdone = 0;
    }
    return;
  }
  else if(pdone == 1)
  {
    p1 = 0;
    p2 = 0;

    is_neg = 0;
    p = 0;
    num_param = 0;

    pdone = 0;
  }
  else if(isinput())
  {
    c = input();
    if(c == '-')
    {
      is_neg = 1;
    }
    else if(c == '.')
    {
      pdone = 100;
    }
    else if(c == ',' || c == ' ' || c == ';' || c == '\n')
    {
      pdone = 0;
      if(!(num_param & 0x80) && c == ' ')
      {
        return;
      }

      if(num_param & 0x80)
      {
        if(is_neg)
        {
          p = -p;
          is_neg = 0;
        }

        if(num_param & 0x01)
        {
          if(pt == QUEUE_XS || pt == QUEUE_XO)
          {
            p2 = p;
          }
          else
          {
            if(queue_rel == 0)
              p += offset_y;
            p2 = (long)(p * scale_y);
            if(queue_rel == 0)
              p2 -= parser_pos_y;
          }
          p = 0;
          num_param = 2;
        }
        else
        {
          if(pt == QUEUE_XS || pt == QUEUE_XO)
          {
            p1 = p;
          }
          else
          {
            if(queue_rel == 0)
               p += offset_x;
            p1 = (long)(p * scale_x); 
            if(queue_rel == 0)
              p1 -= parser_pos_x;
          }
          p = 0;
          num_param = 1;
        }
      }

      if(c == ';' || c == '\n')
        goto DONE;

      if(!(num_param & 0x01))
      {
        if(num_queue < MAX_QUEUE)
          aq();
        else
          pdone = 2;
      }
    }
    else if(c >= '0' && c <= '9')
    {
      if(pdone != 100)
      {
        mul_ten_long(&p);
        p += (c-'0');
        num_param |= 0x80;
      }
    }
  }
  return;
DONE:
  if(!(num_param & 0x01))
  {
    if(pt == QUEUE_XS)
    {
      scale_x = (float)p1 / 10000.0;
      scale_y = (float)p2 / 10000.0;
      if(scale_x < 0)
        scale_x = -scale_x;
      
      if(scale_y < 0)
        scale_y = -scale_y;
      
      cmd_done();
    }
    else if(pt == QUEUE_XO)
    {
      offset_x = p1;
      offset_y = p2;
      cmd_done();
    }
    else if(num_queue < MAX_QUEUE)
    {
      aq();
      cmd_done();
    }
    else
      pdone = 3;
  }
  else
    cmd_done();
}

void parse_in(void)
{
  if(num_queue == 0 && num_steps == 0)
  {
    if(isinput())
    {
      c = input();
      if(c == ';' || c == '\n')
      {
        init_plotter();
        pdone = 1;
        return;
      }
    }
  }
}

void parse(void)
{
  if(pdone == 1 && isinput())
  {
    CMD <<= 8;
    CMD += input();
    CMD &= 0xDFDF;
  }

  if(pdone == 200)
  {
    parse_in();
  }
  else
  {
    switch(CMD)
    {
      case IN:
              parse_in();
              pdone = 200;
              break;
      case FS:
              pt = QUEUE_FS;
              parse_single();
              break;
      case PA:
              queue_rel = 0;
              pt = QUEUE_PA;
              parse_multi();
              break;
      case PD:
              pt = QUEUE_PD;
              parse_multi();
              break;
      case PR:
              queue_rel = 1;
              pt = QUEUE_PR;
              parse_multi();
              break;
      case PU:
              pt = QUEUE_PU;
              parse_multi();
              break;
      case EP:
              pt = QUEUE_EP;
              parse_single();
              break;
      case ED:
              pt = QUEUE_ED;
              parse_multi();
              break;
      case EU:
              pt = QUEUE_EU;
              parse_multi();
              break;
      case MO:
              pt = QUEUE_MO;
              parse_single();
              break;
      case XP:
              pt = QUEUE_XP;
              parse_single();
              break;
      case XS:
              pt = QUEUE_XS;
              parse_multi();
              break;
      case XO:
              pt = QUEUE_XO;
              parse_multi();
              break;
      case XR:
              pt = QUEUE_XR;
              parse_single();
              break;
      case VS:
              pt = QUEUE_VS;
              parse_single();
              break;

      case XX:
              out_num_long(chalk_pos);
              output('\n');
              out_num_byte(servo_high);
              output('\n');
              output('C');
              // enelver car n'utilise pas le chalk SR
              //if(PINC & CHALK_IR)
              //{
              //    output('1');
              //    output('\n');
              //}
              //else
              //{
              //    output('0');
              //    output('\n');
              //}

              CMD = 0x00;
      case ZQ:
              break;
    }
  }
}
