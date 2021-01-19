#ifndef _TIMER_H_
#define _TIMER_H_

#include "fifo.h"

#define PIT_CTRL    0x43
#define PIT_CNT0    0x40
#define FREQ_100HZ  0x2e9c
#define MAX_TIMER   0xff

struct TIMER
{
    unsigned int timeout, flags;
    struct FIFO8 *fifo;
    unsigned char data;
};

struct TIMERCTL
{
    unsigned int count, next, using;
    struct TIMER *timers[MAX_TIMER];
    struct TIMER timers0[MAX_TIMER];
};

void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data);
void timer_free(struct TIMER *timer);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);

#endif
