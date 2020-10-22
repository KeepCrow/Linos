#include "bootpack.h"

#define PORT_KEYDAT     0x0060

struct KEYBUF keybuf;

/* pic的初始化 */
void init_pic(void)
{
    io_out8(PIC0_IMR,  0xff );	/* 禁止所有主PIC中断 */
    io_out8(PIC1_IMR,  0xff );	/* 禁止所有从PIC中断 */

    io_out8(PIC0_ICW1, 0x11 );  /* 边沿触发模式 */
    io_out8(PIC0_ICW2, 0x20 );	/* IRQ0-7由INT20-27接收 */
    io_out8(PIC0_ICW3, 1 << 2);	/* PIC1由IRQ2连接 */
    io_out8(PIC0_ICW4, 0x01 );	/* 无缓冲区模式 */

    io_out8(PIC1_ICW1, 0x11 );	/* 边沿触发模式 */
    io_out8(PIC1_ICW2, 0x28 );	/* IRQ8-15由INT28-2f接收 */
    io_out8(PIC1_ICW3, 2   	);	/* PIC1由IRQ2连接 */
    io_out8(PIC1_ICW4, 0x01 );	/* 无缓冲区模式 */

    io_out8(PIC0_IMR,  0xfb );	/* 11111011 PIC1以外禁止 */	
    io_out8(PIC1_IMR,  0xff );	/* 11111111 禁止所有中断 */
}

/* 来自键盘PS/2的中断 */
void inthandler21(int *esp)
{
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61);
    data = io_in8(PORT_KEYDAT);
    if (keybuf.flag == 0)
    {
        keybuf.flag = 1;
        keybuf.data = data;
    }
    return;
}

void inthandler27(int *esp)
{
    io_out8(PIC0_OCW2, 0x67);   /* 通知PIC"IRQ-07已经受理完毕" */
    return;
}

/* 来自鼠标PS/2的中断 */
void inthandler2c(int *esp)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
    boxfill8(binfo->vram, binfo->scrnx, BLACK, 0, 0, 32 * 8, 16);
    putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, WHITE, "INT 2C (IRQ-12) : PS/2 mouse");
    for (;;)
    {
        io_hlt();
    }
}