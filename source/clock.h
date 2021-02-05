#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "memory.h"

struct CLOCK
{
    int hour, min, sec;
};

static inline void clock_init(struct CLOCK *clock)
{
    clock->hour = clock->min = clock->sec = 0;
}

static inline void clock_next_second(struct CLOCK *clock)
{
    clock->sec += 1;
    if (clock->sec == 60)
    {
        clock->min += 1;
        clock->sec  = 0;
        if (clock->min == 60)
        {
            clock->hour += 1;
            clock->min   = 0;
        }
    }
}

#endif
