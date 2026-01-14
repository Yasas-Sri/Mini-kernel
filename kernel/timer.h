#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern uint32_t PIT_FREQUENCY; // Hz

void timer_init(uint32_t frequency);
void timer_tick(void);
uint32_t timer_get_ticks(void);

#endif
