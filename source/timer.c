#include "timer.h"
#include "naskfunc.h"
#include "int.h"

#define TIMER_FLAGS_FREE    0
#define TIMER_FLAGS_ALLOC   1
#define TIMER_FLAGS_USING   2

struct TIMERCTL timerctl;
struct FIFO8 timerfifo;

void init_pit(void)
{
    int i;

    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, FREQ_100HZ & 0xff);
    io_out8(PIT_CNT0, FREQ_100HZ >> 8);

    timerctl.count = 0;
    timerctl.next  = 0xffffffff;
    timerctl.using = 0;
    for (i = 0; i < MAX_TIMER; i++)
        timerctl.timers0[i].flags = TIMER_FLAGS_FREE;
    return;
}

struct TIMER *timer_alloc(void)
{
    int i;
    for (i = 0; i < MAX_TIMER; i++)
    {
        if (timerctl.timers0[i].flags == TIMER_FLAGS_FREE)
        {
            timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
            return &(timerctl.timers0[i]);
        }
    }
    return 0;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data)
{
    timer->fifo = fifo;
    timer->data = data;
    return;
}

void timer_free(struct TIMER *timer)
{
    timer->flags = TIMER_FLAGS_FREE;
    return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
    int eflags, i = 0, j = 0;
    timer->timeout = timerctl.count + timeout;
    timer->flags   = TIMER_FLAGS_USING;

    eflags = io_load_eflags();
    io_cli();
    while (i < timerctl.using && timerctl.timers[i]->timeout < timerctl.count)
        i += 1;

    for (j = timerctl.using; j > i; j--)
        timerctl.timers[j] = timerctl.timers[j - 1];
    timerctl.using    += 1;
    timerctl.timers[i] = timer;
    timerctl.next      = timerctl.timers[i]->timeout;

    io_store_eflags(eflags);
    return;
}

void inthandler20(int *esp)
{
    int i, j;

    io_out8(PIC0_OCW2, 0x60);
    
    timerctl.count += 1;
    if (timerctl.next > timerctl.count)
        return;

    for (i = 0; i < timerctl.using; i++)
    {
        if (timerctl.timers[i]->timeout > timerctl.count)
            break;
        timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
        fifo8_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
    }

    timerctl.using -= i;
    for (j = 0; j < timerctl.using; j++)
        timerctl.timers[j] = timerctl.timers[i + j];
    timerctl.next = (timerctl.using > 0) ? timerctl.timers[0]->timeout : 0xffffffff;
    return;
}
