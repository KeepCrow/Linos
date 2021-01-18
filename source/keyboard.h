#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

void wait_KBC_sendready(void);
void init_keyboard(void);
void inthandler21(int *esp);

#endif
