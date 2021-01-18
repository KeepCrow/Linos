#include "timer.h"
#include "naskfunc.h"
#include "int.h"

struct TIMERCTL timerctl;

void init_pit(void)
{
    io_out8(PIT_CTRL, 0x34);
    io_out8(PIT_CNT0, FREQ_100HZ & 0xff);
    io_out8(PIT_CNT0, FREQ_100HZ >> 8);
    timerctl.count = 0;
}

void inthandler20(int *esp)
{
    io_out8(PIC0_OCW2, 0x60);
    timerctl.count += 1;
    return;
}
