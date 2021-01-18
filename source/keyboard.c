#include "keyboard.h"
#include "int.h"
#include "fifo.h"

struct FIFO8 keyfifo;

void wait_KBC_sendready(void)
{
    /* 等待keyboard controller准备完毕 */
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0);
    return;
}

void init_keyboard(void)
{
    /* 初始化keyborad controller */
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

/* 来自键盘PS/2的中断 */
void inthandler21(int *esp)
{
    io_out8(PIC0_OCW2, 0x61);
    fifo8_put(&keyfifo, io_in8(PORT_KEYDAT));
    return;
}
