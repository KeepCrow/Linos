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

void enable_mouse(struct MOUSE_DEC *mdec)
{
    /* 激活鼠标 */
    wait_KEB_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KEB_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);  /* 这一句执行完毕后，鼠标会返回0xfa */
    mdec->phase = 0;
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
    if (dat == 0xfa)
        return 0;

    if (mdec->phase == 0)
    {
        if ((dat & 0xc8) == 0x08)
        {
            mdec->buf[mdec->phase] = dat;
            mdec->phase = 1;
        }
        return 0;
    }
    else if (mdec->phase == 1)
    {
        mdec->buf[mdec->phase] = dat;
        mdec->phase = 2;
        return 0;
    }
    else
    {
        mdec->buf[mdec->phase] = dat;
        mdec->phase = 0;
        mdec->btn = mdec->buf[0] & 0x07;    /* 111 */
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];

        /* https://www.cnblogs.com/qiweiwang/archive/2011/03/21/1990346.html
         * bit7: Y overflow     bit6: Y overflow        bit5: Y sign bit     
         * bit4: X sign bit     bit3: always 1          bit2: middle btn
         * bit1: right btn      bit0: left btn
         */
        if ((mdec->buf[0] & 0x10) != 0)
            mdec->x |= 0xffffff00;
        if ((mdec->buf[0] & 0x20) != 0)
            mdec->y |= 0xffffff00;
        mdec->y = -mdec->y; /* 鼠标的y方向与画面相反 */

        return 1;
    }

    return -1;
}

/* 暂时无法修改为LinMain() */
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
    struct MOUSE_DEC mdec;

    init_gdtidt();  /* 初始化gdt与idt */
    init_pic(); /* 初始化pic */
    init_keyboard();    /* 初始化键盘 */
    enable_mouse(&mdec); /* 使能鼠标 */
    io_sti();

    init_palette(); /* 设置调色板 */
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, BACK_COLOR);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

    /* 修改PIC以接收中断信号 */
    io_out8(PIC0_IMR, 0xf9);    /* 11111001 */
    io_out8(PIC1_IMR, 0xef);    /* 11101111 */

    fifo8_init(&keyfifo, keyfifo_buf, 32);
    fifo8_init(&mousefifo, mousefifo_buf, 128);
    while (1)
    {
        io_cli();
        if (fifo8_status(&keyfifo) != 0)
        {
            /* 输出字符 */
            io_sti();
            sprintf(msg, "keyboard input: %02X", fifo8_get(&keyfifo));
            show_line8(binfo->vram, binfo->scrnx, LN_KEYBOARD, msg);
        }
        if (fifo8_status(&mousefifo) != 0)
        {
            io_sti();
            if (mouse_decode(&mdec, fifo8_get(&mousefifo)) == 1)
            {
                /* 刷新鼠标图片 */
                boxfill8(binfo->vram, binfo->scrnx, BACK_COLOR, mx, my, 16, 16);
                mx += mdec.x;
                my += mdec.y;
                if (mx < 0) mx = 0;
                if (my < 0) my = 0;
                if (mx > binfo->scrnx - 16) mx = binfo->scrnx - 16;
                if (my > binfo->scrny - 16) my = binfo->scrny - 16;
                putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

                /* 显示鼠标状态 */
                sprintf(msg, "mouse status: (lcr, %d, %d)", mx, my);
                if ((mdec.btn & 0x01) != 0)
                    msg[15] = 'L';
                else if ((mdec.btn & 0x02) != 0)
                    msg[17] = 'R';
                else if ((mdec.btn & 0x04) != 0)
                    msg[16] = 'C';
                show_line8(binfo->vram, binfo->scrnx, LN_MOUSE, msg);
            }
        }
        io_stihlt();
    }
}
