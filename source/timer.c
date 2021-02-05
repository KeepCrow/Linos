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

    timerctl.count     = 0;
    for (i = 0; i < MAX_TIMER; i++)
        timerctl.timers0[i].flags = TIMER_FLAGS_FREE;
    
    /* 前`TIMER_HASH_SIZE`个timer用作哨兵 */
    for (i = 0; i < TIMER_HASH_SIZE; i++)
    {
        timerctl.timers[i]             = &timerctl.timers0[i];
        timerctl.timers[i]->flags      = TIMER_FLAGS_USING;
        timerctl.timers[i]->timeout    = 0xffffffff;
        timerctl.timers[i]->next_timer = 0;
    }

    return;
}

struct TIMER *timer_alloc(void)
{
    int i;
    /* 前`TIMER_HASH_SIZE`个timer都是哨兵 */
    for (i = TIMER_HASH_SIZE; i < MAX_TIMER; i++)
    {
        if (timerctl.timers0[i].flags == TIMER_FLAGS_FREE)
        {
            timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
            return &(timerctl.timers0[i]);
        }
    }
    return 0;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
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
    int eflags;
    struct TIMER *pre, *cur;

    timer->timeout = timerctl.count + timeout;
    pre = timerctl.timers[timer->timeout % TIMER_HASH_SIZE];
    cur = pre->next_timer;
    timer->flags = TIMER_FLAGS_USING;

    eflags = io_load_eflags();
    io_cli();

    if (timer->timeout < pre->timeout)
    {
        timer->next_timer = pre;
        timerctl.timers[timer->timeout % TIMER_HASH_SIZE] = timer;
        io_store_eflags(eflags);
        return;
    }

    /* 找到插入点 */
    while (timer->timeout > cur->timeout)
    {
        pre = cur;
        cur = cur->next_timer;
    }
    pre->next_timer   = timer;
    timer->next_timer = cur;
    io_store_eflags(eflags);

    return;
}

void inthandler20(int *esp)
{
    struct TIMER *head;

    io_out8(PIC0_OCW2, 0x60);
    
    timerctl.count += 1;
    head = timerctl.timers[timerctl.count % TIMER_HASH_SIZE];
    
    while (head->timeout <= timerctl.count)
    {
        head->flags = TIMER_FLAGS_ALLOC;
        fifo32_put(head->fifo, head->data);
        head = head->next_timer;
    }
    timerctl.timers[timerctl.count % TIMER_HASH_SIZE] = head;
    return;
}
