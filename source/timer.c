#include "timer.h"
#include "naskfunc.h"
#include "int.h"

#define TIMER_FLAGS_FREE    0
#define TIMER_FLAGS_ALLOC   1
#define TIMER_FLAGS_USING   2

struct TIMERCTL timerctl;

void init_pit(void)
{
    int i;

    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, FREQ_100HZ & 0xff);
    io_out8(PIT_CNT0, FREQ_100HZ >> 8);

    timerctl.count = 0;
    for (i = 0; i < MAX_TIMER; i++)
        timerctl.timers[i].flags = TIMER_FLAGS_FREE;
}

struct TIMER *timer_alloc(void)
{
    int i;
    for (i = 0; i < MAX_TIMER; i++)
    {
        if (timerctl.timers[i].flags == TIMER_FLAGS_FREE)
        {
            timerctl.timers[i].flags = TIMER_FLAGS_ALLOC;
            return &(timerctl.timers[i]);
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
    timer->timeout = timeout;
    timer->flags   = TIMER_FLAGS_USING;
    return;
}

void inthandler20(int *esp)
{
    int i;
    struct TIMER *timer;

    io_out8(PIC0_OCW2, 0x60);
    
    timerctl.count += 1;
    for (i = 0; i < MAX_TIMER; i++)
    {
        timer = &(timerctl.timers[i]);
        if (timer->flags == TIMER_FLAGS_USING)
        {
            timer->timeout -= 1;
            if (timer->timeout == 0)
            {
                fifo8_put(timer->fifo, timer->data);
                timer->flags = TIMER_FLAGS_ALLOC;
            }
        }
    }
    return;
}
