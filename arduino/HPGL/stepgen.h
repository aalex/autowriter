#ifndef _STEPGEN_H_
#define _STEPGEN_H_

#define MAX_STEPS  64     // must be power of two!

void init_stepgen(void);
void read_next_step(void);
void write_next_step(uint16_t);
void gen_steps(void);
void get_next_step(void);

#endif

