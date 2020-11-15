#include <stdio.h>
#include <string.h>
#include "bootpack.h"
#include "memory.h"

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

/* 暂时无法修改为LinMain() */
void HariMain(void)
{
    // char logo[16];
    char msg[32];
    char mouse[256];
    char keyfifo_buf[32];
    char mousefifo_buf[128];
    // int logox, logoy;
    int i, mem_total, mx, my;
    // int psize = 5;
    // int data;
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct MEMMAN *man = (struct MEMMAN *)MEMMAN_ADR;
    struct MOUSE_DEC mdec;

    init_gdtidt();  /* 初始化gdt与idt */
    init_pic(); /* 初始化pic */
    io_sti();
    init_keyboard();    /* 初始化键盘 */
    enable_mouse(&mdec); /* 使能鼠标 */
    io_sti();

    init_palette(); /* 设置调色板 */
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, BACK_COLOR);
    fifo8_init(&keyfifo, keyfifo_buf, 32);
    fifo8_init(&mousefifo, mousefifo_buf, 128);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

    /* 内存显示 */
    mem_total = memtest(0x00400000, 0xbfffffff);
    memman_init(man);
    memman_free(man, 0x00001000, 0x0009e000);
    memman_free(man, 0x00400000, mem_total - 0x00400000);
    sprintf(msg, "memory %dMB free: %dKB", 
            mem_total >> 20, memman_total(man) >> 10);
    show_line8(binfo->vram, binfo->scrnx, LN_MEM, msg);

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
