#include "keyboard.h"
#include "int.h"
#include "naskfunc.h"

// struct FIFO8 keyfifo;
struct FIFO32 *keyfifo;
int keydata0;

void wait_KBC_sendready(void)
{
    /* 等待keyboard controller准备完毕 */
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0);
    return;
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
    /* 初始化keyborad controller */
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);

    keyfifo  = fifo;
    keydata0 = data0;
    return;
}

/* 来自键盘PS/2的中断 */
void inthandler21(int *esp)
{
    int data;
    io_out8(PIC0_OCW2, 0x61);
    fifo32_put(keyfifo, io_in8(PORT_KEYDAT) + FIFOVAL_KEY_BASE);
    return;
}
