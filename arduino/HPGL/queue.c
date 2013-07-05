#include <inttypes.h>
#include "HPGL.h"
#include "sio_isr.h"
#include "queue.h"

uint8_t                 cur_queue_read;
uint8_t                 cur_queue_write;
uint8_t                 num_queue;
queueEntry              queue[MAX_QUEUE];

extern int32_t          p1;
extern int32_t          p2;
extern uint8_t          pt;

extern union longbytes  x;
extern union longbytes  y;
extern uint8_t          qt;

void init_queue(void)
{
  uint8_t cnt;
  
  for(cnt = 0; cnt < MAX_QUEUE; cnt++)
  { 
    queue[cnt].type    = 0x00;
    queue[cnt].point.x = 0x00;
    queue[cnt].point.y = 0x00;
  } 
  
  cur_queue_write = 0;
  cur_queue_read  = 0;
  num_queue       = 0;
}

void add_queue()
{
  queue[cur_queue_write].type    = pt;
  queue[cur_queue_write].point.x = p1;
  queue[cur_queue_write].point.y = p2;
  
  cur_queue_write++;
  cur_queue_write &= (MAX_QUEUE-1);
  
  num_queue++;
}

void poke_queue(uint8_t type, int32_t x, int32_t y)
{
  uint8_t cq;
  cq = cur_queue_read;
  cq--;
  cq &= (MAX_QUEUE-1);
  
  queue[cq].type    = type;
  queue[cq].point.x = x;
  queue[cq].point.y = y;
}

void read_queue_s()
{
  if(num_queue != 0)
  {
    qt = queue[cur_queue_read].type;
    x.number = queue[cur_queue_read].point.x;
    y.number = queue[cur_queue_read].point.y;
    queue[cur_queue_read].type = QUEUE_IDLE;
    cur_queue_read++;
    cur_queue_read &= (MAX_QUEUE-1);
    num_queue--;
  }
}
