#ifndef _QUEUE_H_
#define _QUEUE_H_

#define MAX_QUEUE   32

#define QUEUE_IDLE         00

#define QUEUE_PA           1
#define QUEUE_PD           2
#define QUEUE_PR           3
#define QUEUE_PU           4
#define QUEUE_VS           7
#define QUEUE_FS           8
#define QUEUE_EP           9
#define QUEUE_ED           10
#define QUEUE_EU           11
#define QUEUE_MO           12
#define QUEUE_XP           13   // not really queued, active immediatly
#define QUEUE_XS           14   // not really queued, active immediatly
#define QUEUE_XO           15   // not really queued, active immediatly
#define QUEUE_XR           16   // not really queued, active immediatly

typedef struct pntStruct
{
  int32_t x;
  int32_t y;
} pnt;

typedef struct queueEntryStruct
{
  uint8_t type;
  pnt     point;
} queueEntry;

void     init_queue(void);
void     add_queue(void);
void     poke_queue(uint8_t, int32_t, int32_t);
void     read_queue_s(void);

#endif
