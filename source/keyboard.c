#include "keyboard.h"
#include "int.h"

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