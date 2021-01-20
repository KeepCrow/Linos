#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "fifo.h"

#define FIFOVAL_MOUSE_BASE  512
#define FIFOVAL_MOUSE_MAX   767

struct MOUSE_DEC
{
    unsigned char buf[3];
    unsigned char phase;
    int x, y;
    int btn;
};

void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
void init_mouse_cursor8(char *mouse, char bc);
void inthandler2c(int *esp);

#endif
