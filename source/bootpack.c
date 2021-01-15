#include <stdio.h>
#include <string.h>
#include "bootpack.h"

// #define bootpack_debug

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

/* 暂时无法修改为LinMain() */
void HariMain(void)
{
    // char logo[16];
    char msg[32];
    char bufmouse[256];
    char keyfifo_buf[32];
    char mousefifo_buf[128];
    // int logox, logoy;
    int mem_total, mx, my;
    // int psize = 5;
    // int data;
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct MEMMAN *man = (struct MEMMAN *)MEMMAN_ADR;
    struct MOUSE_DEC mdec;
    struct SHTCTL *shtctl;
    struct SHEET *shtback, *shtmouse; 
    unsigned char *bufback;

    init_gdtidt();  /* 初始化gdt与idt */
    init_pic(); /* 初始化pic */
    io_sti();
    fifo8_init(&keyfifo, keyfifo_buf, 32);
    fifo8_init(&mousefifo, mousefifo_buf, 128);
    io_out8(PIC0_IMR, 0xf9);    /* 11111001 */
    io_out8(PIC1_IMR, 0xef);    /* 11101111 */
    init_palette(); /* 设置调色板 */

    init_keyboard();    /* 初始化键盘 */
    mdec.phase = 0;
    enable_mouse(&mdec); /* 使能鼠标 */
    mem_total = memtest(0x00400000, 0xbfffffff);
    memman_init(man);
    memman_free(man, 0x00001000, 0x0009e000);
    memman_free(man, 0x00400000, mem_total - 0x00400000);

    bufback = memman_alloc4k(man, binfo->scrnx * binfo->scrny);
    init_screen8(bufback, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(bufmouse, INVI_COLOR);

    shtctl = shtctl_init(man, binfo->vram, binfo->scrnx, binfo->scrny);
    shtback = sheet_alloc(shtctl);
    sheet_setbuf(shtback, bufback, binfo->scrnx, binfo->scrny, -1);
    sheet_slide(shtctl, shtback, 0, 0);
#ifndef bootpack_debug
    sheet_updown(shtctl, shtback, 0);
#endif
    shtmouse = sheet_alloc(shtctl);
    sheet_setbuf(shtmouse, bufmouse, 16, 16, INVI_COLOR);
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    sheet_slide(shtctl, shtmouse, mx, my);
    sheet_updown(shtctl, shtmouse, 1);

    /* 内存显示 */
    sprintf(msg, "memory %dMB free: %dKB", mem_total >> 20, memman_total(man) >> 10);
    show_line8(bufback, binfo->scrnx, LN_MEM, msg);

#ifdef bootpack_debug
    debug_printf("shtback:%p[h:%d,x:%d,y:%d]", shtback, shtback->height, shtback->vx0, shtback->vy0);
    debug_printf("shtmouse:%p[h:%d,x:%d,y:%d]", shtmouse, shtmouse->height, shtmouse->vx0, shtmouse->vy0);
    debug_printf("vram:%p", binfo->vram);
    debug_printf("mouse:[x:%d,y:%d]", mx, my);
#endif

    sheet_refresh(shtctl, shtback, 0, 0, binfo->scrnx, binfo->scrny);

    while (1)
    {
        io_cli();
        if (fifo8_status(&keyfifo) != 0)
        {
            /* 输出字符 */
            io_sti();
            sprintf(msg, "keyboard input: %02X", fifo8_get(&keyfifo));
            show_line8(bufback, binfo->scrnx, LN_KEYBOARD, msg);
            sheet_refresh(shtctl, shtback, 0, LN_KEYBOARD * LINE_SPAN, binfo->scrnx, (LN_KEYBOARD + 1) * LINE_SPAN);
        }
        else if (fifo8_status(&mousefifo) != 0)
        {
            io_sti();
            if (mouse_decode(&mdec, fifo8_get(&mousefifo)) == 1)
            {
                mx += mdec.x;
                my += mdec.y;
                if (mx < 0) mx = 0;
                if (my < 0) my = 0;
                if (mx > binfo->scrnx - 16) mx = binfo->scrnx - 16;
                if (my > binfo->scrny - 16) my = binfo->scrny - 16;

                /* 显示鼠标状态 */
                sprintf(msg, "mouse status: (lcr, %d, %d)", mx, my);
                if ((mdec.btn & 0x01) != 0)
                    msg[15] = 'L';
                else if ((mdec.btn & 0x02) != 0)
                    msg[17] = 'R';
                else if ((mdec.btn & 0x04) != 0)
                    msg[16] = 'C';
                show_line8(bufback, binfo->scrnx, LN_MOUSE, msg);
                sheet_refresh(shtctl, shtback, 0, LN_MOUSE * LINE_SPAN, binfo->scrnx, (LN_MOUSE + 1) * LINE_SPAN);

#ifdef bootpack_debug
                debug_printf("mouse[%d, %d]", mx, my);
#endif
                sheet_slide(shtctl, shtmouse, mx, my);
            }
        }
        io_stihlt();
    }
}
