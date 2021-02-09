#include <stdio.h>
#include <string.h>
#include "clock.h"
#include "window.h"

void clock_next_second(struct CLOCK *clock)
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

void show_clock(struct CLOCK *clock, struct SHEET *sht)
{
    int time_str_len = 8;
    struct MEMMAN *man = (struct MEMMAN *)MEMMAN_ADR;
    char *time = (char *)memman_alloc(man, time_str_len + 1);

    boxfill8(sht->buf, sht->bxsize, CLOCK_X0(sht->bxsize), CLOCK_Y0(sht->bysize), CLOCK_XLEN, CLOCK_YLEN, DARK_WHITE, DARK_WHITE);
    sprintf(time, "%02d:%02d:%02d", clock->hour, clock->min, clock->sec);
    putfonts8_asc(sht->buf, sht->bxsize, CLOCK_X0(sht->bxsize), CLOCK_Y0(sht->bysize), FONT_COLOR, time);
    sheet_refresh(sht, CLOCK_X0(sht->bxsize), CLOCK_Y0(sht->bysize), CLOCK_XLEN, CLOCK_YLEN);
}
