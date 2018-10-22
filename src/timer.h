#ifndef IB_TIMER
#define IB_TIMER

/* short file with utilities for basic timers */

#include <stdint.h>

typedef uint32_t ib_timepoint;

ib_timepoint ib_timer_point();
int ib_timer_ms(ib_timepoint since);

#endif
