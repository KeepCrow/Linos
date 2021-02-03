#include <stdio.h>
#include <string.h>
#include "bootpack.h"
#include "dsctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "naskfunc.h"
#include "sheet.h"
#include "window.h"
#include "timer.h"
#include "task.h"

#define FIFOVAL_5SECOND     5
#define FIFOVAL_3SECOND     3
#define FIFOVAL_SHINING0    0
#define FIFOVAL_SHINING1    1

// #define bootpack_debug
extern struct TIMERCTL timerctl;
extern struct TSS32 tss_a, tss_b;

static char keytable[0x54] = {
     0,   0,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^',  0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[',  0,   0,  'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':',  0,   0,  ']', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/',  0,  '*',  0,  ' ',  0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,  '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};

/**
 * @param bc 更新位置的背景色
 * @param mc 更新位置字体颜色
 */
static inline void update_msg(struct SHEET *sht, char *msg, int x0, int y0, int xl, int yl, char bc, char mc)
{
    boxfill8(sht->buf, sht->bxsize, x0, y0, xl, yl, bc, bc);
    putfonts8_asc(sht->buf, sht->bxsize, x0, y0, mc, msg);
    sheet_refresh(sht, x0, y0, xl, yl);
}

/* 暂时无法修改为LinMain() */
void HariMain(void)
{
    char cursor[2] = {0xdd, 0};
    unsigned char msg[32], bufmouse[256];
    unsigned char *bufback, *bufwin;
    int mem_total, mx, my, cursori = 0;
    int data;
    int fifobuf[128];
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    struct MEMMAN *man = (struct MEMMAN *)MEMMAN_ADR;
    struct MOUSE_DEC mdec;
    struct SHTCTL *shtctl;
    struct SHEET *shtback, *shtmouse, *shtwin;
    struct TIMER *timer0, *timer1, *timer2;
    struct FIFO32 fifo;
    struct WINDOW win1;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

    init_gdtidt();  /* 初始化gdt与idt */
    init_pic(); /* 初始化pic */
    io_sti();
    
    fifo32_init(&fifo, fifobuf, 128);

    init_pit();
    timer0 = timer_alloc();
    timer_init(timer0, &fifo, FIFOVAL_5SECOND);
    timer_settime(timer0, 500);
    timer1 = timer_alloc();
    timer_init(timer1, &fifo, FIFOVAL_3SECOND);
    timer_settime(timer1, 300);
    timer2 = timer_alloc();
    timer_init(timer2, &fifo, FIFOVAL_SHINING1);
    timer_settime(timer2, 50);

    io_out8(PIC0_IMR, 0xf8);    /* 11111000 */
    io_out8(PIC1_IMR, 0xef);    /* 11101111 */
    init_palette(); /* 设置调色板 */

    init_keyboard(&fifo, FIFOVAL_KEY_BASE);    /* 初始化键盘 */
    mdec.phase = 0;
    enable_mouse(&fifo, FIFOVAL_MOUSE_BASE, &mdec); /* 使能鼠标 */
    mem_total = memtest(0x00400000, 0xbfffffff);
    memman_init(man);
    memman_free(man, 0x00001000, 0x0009e000);
    memman_free(man, 0x00400000, mem_total - 0x00400000);

    bufback = (unsigned char *)memman_alloc4k(man, binfo->scrnx * binfo->scrny);
    bufwin  = (unsigned char *)memman_alloc4k(man, 160 * 52);
    init_screen8(bufback, binfo->scrnx, binfo->scrny);
    init_window8(&win1, bufwin, "window", 160, 52, DARK_BLUE, BRIGHT_GRAY);
    make_window8(&win1);

    init_mouse_cursor8(bufmouse, INVI_COLOR);

    shtctl = shtctl_init(man, binfo->vram, binfo->scrnx, binfo->scrny);
    shtback = sheet_alloc(shtctl);
    sheet_setbuf(shtback, bufback, binfo->scrnx, binfo->scrny, -1);
    sheet_slide(shtback, 0, 0);
#ifndef bootpack_debug
    sheet_updown(shtback, 0);
#endif
    shtwin = sheet_alloc(shtctl);
    sheet_setbuf(shtwin, bufwin, 160, 52, -1);
    sheet_slide(shtwin, 80, 72);
    sheet_updown(shtwin, 1);
    shtmouse = sheet_alloc(shtctl);
    sheet_setbuf(shtmouse, bufmouse, 16, 16, INVI_COLOR);
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    sheet_slide(shtmouse, mx, my);
    sheet_updown(shtmouse, 2);

    /* 内存显示 */
    sprintf(msg, "memory %dMB free: %dKB", mem_total >> 20, memman_total(man) >> 10);
    line_show8(shtback, LN_MEM, msg);
    sheet_refresh(shtback, 0, 0, binfo->scrnx, binfo->scrny);

    tss_init(&tss_a);
    set_segmdesc(gdt + 3, 103, (int) &tss_a, AR_TSS32);
    tss_init(&tss_b);
    set_segmdesc(gdt + 4, 103, (int) &tss_b, AR_TSS32);

    while (1)
    {
        io_cli();
        if (fifo32_status(&fifo) == 0)
        {
            io_sti();
            continue;
        }

        data = fifo32_get(&fifo);
        io_sti();

        if (data >= FIFOVAL_KEY_BASE && data <= FIFOVAL_KEY_MAX) /* 键盘数据 */
        {
            sprintf(msg, "keyboard input: %02X", data - FIFOVAL_KEY_BASE);
            line_show8(shtback, LN_KEYBOARD, msg);
            if (data < FIFOVAL_KEY_BASE + 0x54)
            {
                msg[0] = keytable[data - FIFOVAL_KEY_BASE];
                msg[1] = 0;
                update_msg(shtwin, msg, cursori * FONT_WIDTH + 2, TITLE_HEIGHT + 3, FONT_WIDTH, FONT_HEIGHT, WHITE, BLACK);
                cursori += 1;
                if (cursori >= shtwin->bxsize / FONT_WIDTH - 1)
                    cursori = 0;
            }
        }
        else if ((data >= FIFOVAL_MOUSE_BASE) && (data <= FIFOVAL_MOUSE_MAX))   /* 鼠标数据 */
        {
            if (mouse_decode(&mdec, data - FIFOVAL_MOUSE_BASE) == 1)
            {
                mx += mdec.x;
                my += mdec.y;
                if (mx < 0) mx = 0;
                if (my < 0) my = 0;
                if (mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
                if (my > binfo->scrny - 1) my = binfo->scrny - 1;

                /* 显示鼠标状态 */
                sprintf(msg, "mouse status: (lcr, %d, %d)", mx, my);
                if ((mdec.btn & 0x01) != 0)
                    msg[15] = 'L';
                else if ((mdec.btn & 0x02) != 0)
                    msg[17] = 'R';
                else if ((mdec.btn & 0x04) != 0)
                    msg[16] = 'C';
                line_show8(shtback, LN_MOUSE, msg);
                sheet_slide(shtmouse, mx, my);

                /* 移动窗口 */
                if ((mdec.btn & 0x01) != 0)
                    sheet_slide(shtwin, mx - 80, my - 8);
            }
        }
        else if (data == FIFOVAL_3SECOND)
        {
            line_show8(shtback, (enum LineNum)3, "3[sec]");
        }
        else if (data == FIFOVAL_5SECOND)
        {
            line_show8(shtback, (enum LineNum)3, "5[sec]");
        }
        else if (data == FIFOVAL_SHINING0)
        {
            update_msg(shtwin, " ", cursori * FONT_WIDTH + 2, TITLE_HEIGHT + 3, FONT_WIDTH, FONT_HEIGHT, WHITE, BLACK);
            timer_init(timer2, &fifo, FIFOVAL_SHINING1);
            timer_settime(timer2, 50);
        }
        else if (data == FIFOVAL_SHINING1)
        {
            update_msg(shtwin, cursor, cursori * FONT_WIDTH + 2, TITLE_HEIGHT + 3, FONT_WIDTH, FONT_HEIGHT, WHITE, BLACK);
            timer_init(timer2, &fifo, FIFOVAL_SHINING0);
            timer_settime(timer2, 50);
        }
    }
}
