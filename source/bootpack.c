#include <stdio.h>
#include <string.h>
#include "bootpack.h"

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

void wait_KEB_sendready(void)
{
    /* 等待keyboard controller准备完毕 */
    while ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) != 0);
    return;
}

void init_keyboard(void)
{
    /* 初始化keyborad controller */
    wait_KEB_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    wait_KEB_sendready();
    io_out8(PORT_KEYDAT, KBC_MODE);
    return;
}

void enable_mouse(void)
{
    /* 激活鼠标 */
    wait_KEB_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KEB_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);  /* 这一句执行完毕后，鼠标会返回0xfa */
    return;
}

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    char logo[16];
    char msg[32];
    char mouse[256];
    char keyfifo_buf[32];
    char mousefifo_buf[128];
    int logox, logoy;
    int i;
    int mx, my;
    int psize = 5;
    int data;
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;

    fifo8_init(&keyfifo, keyfifo_buf, 32);
    fifo8_init(&mousefifo, mousefifo_buf, 128);

    init_gdtidt();  /* 初始化gdt与idt */
    init_pic(); /* 初始化pic */
    init_keyboard();    /* 初始化键盘 */
    enable_mouse(); /* 使能鼠标 */
    io_sti();

    init_palette(); /* 设置调色板 */
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, BACK_COLOR);

    // sprintf(logo, "%msg", "LinOS");
    // logox = (binfo->scrnx - strlen(logo) * 8 * psize) / 2;
    // logoy = (binfo->scrny - 30 - 16 * psize) / 2;
    // show_logo8(binfo->vram, binfo->scrnx, logox, logoy, WHITE, LIGHT_DARK_BLUE, logo, psize);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

    sprintf(msg, "mouse coordinate:(%d, %d)", mx, my);
    // putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, WHITE, msg);
    // show_line8(binfo->vram, binfo->scrnx, LN_MOUSE, msg);

    /* 修改PIC以接收中断信号 */
    io_out8(PIC0_IMR, 0xf9);    /* 11111001 */
    io_out8(PIC1_IMR, 0xef);    /* 11101111 */

    while (1)
    {
        io_cli();
        if (fifo8_status(&keyfifo) != 0)
        {
            /* 输出字符 */
            io_sti();
            sprintf(msg, "keyboard input:%02X", fifo8_get(&keyfifo));
            show_line8(binfo->vram, binfo->scrnx, LN_KEYBOARD, msg);
        }
        if (fifo8_status(&mousefifo) != 0)
        {
            io_sti();
            sprintf(msg, "mouse input:%02X", fifo8_get(&mousefifo));
            show_line8(binfo->vram, binfo->scrnx, LN_MOUSE, msg);
        }
        io_stihlt();
    }
}
