#include <stdio.h>
#include <string.h>
#include "bootpack.h"

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

int load_cr0();
void store_cr0(int cr0);
unsigned int memtest_sub(unsigned int start, unsigned int end);

/* 判断CPU型号是否为486 */
char is486(void)
{
    char flg486;
    unsigned int eflg;

    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    eflg = io_load_eflags();
    flg486 = (eflg & EFLAGS_AC_BIT != 0) ? 1 : 0;

    /* 恢复原本的eflags寄存器值 */
    eflg &= ~EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    return flg486;
}

void disable_cache(char flg486)
{
    unsigned int cr0;
    if (flg486 == 1)
    {
        cr0 = load_cr0();
        cr0 |= CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }
}

void enable_cache(char flg486)
{
    unsigned int cr0;
    if (flg486 == 1)
    {
        cr0 = load_cr0();
        cr0 &= ~CR0_CACHE_DISABLE;
        store_cr0(cr0);
    }
}

// char test_block(unsigned int *address)
// {
//     unsigned int old = *address, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
    
//     /* 第一次试写 */
//     *address = pat0;
//     *address ^= 0xffffffff;
//     if (*address != pat1)
//         return 0;
        
//     /* 第二次试写 */
//     *address ^= 0xffffffff;
//     if (*address != pat0)
//         return 0;
    
//     /* 还原内存块的值 */
//     *address = old;
//     return 1;
// }

// unsigned int memtest_sub(unsigned int start, unsigned int end)
// {
//     unsigned int i, j;
//     for (i = start, j = 0; i <= end; i += 0x100000, j++)
//         if (test_block((unsigned int *)(i + 0xffc)) == 0)
//             break;
//     return j;
// }

unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486;
    unsigned int mem_size = 0;

    flg486 = is486();
    disable_cache(flg486);
    mem_size = memtest_sub(start, end);
    enable_cache(flg486);

    return mem_size;
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
    i = memtest(0x00400000, 0xbfffffff);
    sprintf(msg, "memory %dMB", i);
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
