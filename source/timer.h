#ifndef _TIMER_H_
#define _TIMER_H_

#define PIT_CTRL    0x43
#define PIT_CNT0    0x40
#define FREQ_100HZ  0x2e9c

struct TIMERCTL
{
    unsigned int count;
};

void init_pit(void);
void inthandler20(int *esp);

#endif
