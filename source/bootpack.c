#include <stdio.h>
#include <string.h>
#include "bootpack.h"

extern struct KEYBUF keybuf;

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    char logo[16];
    char msg[10];
    char mouse[256];
    int logox, logoy;
    int mx, my;
    int psize = 5;
    int data;
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;

    init_gdtidt();
    init_pic();
    io_sti();

    init_palette(); /* 设置调色板 */
    init_screen8(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, LIGHT_DARK_BLUE);

    // sprintf(logo, "%msg", "LinOS");
    // logox = (binfo->scrnx - strlen(logo) * 8 * psize) / 2;
    // logoy = (binfo->scrny - 30 - 16 * psize) / 2;
    // show_logo8(binfo->vram, binfo->scrnx, logox, logoy, WHITE, LIGHT_DARK_BLUE, logo, psize);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

    sprintf(msg, "mouse coordinate: (%d, %d)", mx, my);
    // putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, WHITE, msg);
    show_line8(binfo->vram, binfo->scrnx, LN_MOUSE, msg);

    /* 修改PIC以接收中断信号 */
    io_out8(PIC0_IMR, 0xf9);    /* 11111001 */
    io_out8(PIC1_IMR, 0xef);    /* 11101111 */

    for (;;)
    {
        io_cli();
        if (keybuf.flag == 0)
        {
            io_stihlt();
        }
        else
        {
            keybuf.flag = 0;
            io_sti();
            sprintf(msg, "keyboard input: %02X", keybuf.data);
            show_line8(binfo->vram, binfo->scrnx, LN_KEYBOARD, msg);
        }
    }
}
