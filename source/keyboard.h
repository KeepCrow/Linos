#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "fifo.h"

#define FIFOVAL_KEY_BASE    256
#define FIFOVAL_KEY_MAX     511

void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
void inthandler21(int *esp);

#endif
