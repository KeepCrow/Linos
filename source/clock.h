#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "memory.h"
#include "sheet.h"

#define CLOCK_X0(xsize)     ((xsize - 10 * FONT_WIDTH) / 2)
#define CLOCK_Y0(ysize)     ((TITLE_HEIGHT - FONT_HEIGHT) / 2)
#define CLOCK_XLEN          (10 * FONT_WIDTH)
#define CLOCK_YLEN          (FONT_HEIGHT)

struct CLOCK
{
    int hour, min, sec;
};

static inline void clock_init(struct CLOCK *clock)
{
    clock->hour = clock->min = clock->sec = 0;
}

void clock_next_second(struct CLOCK *clock);
void show_clock(struct CLOCK *clock, struct SHEET *sht);

#endif
