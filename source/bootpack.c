#include <stdio.h>
#include "bootpack.h"

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    char logo[16];
    char s[10];
    char mouse[256];
    int logox, logoy;
    int mx, my;
    int psize = 5;
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;

    init_pic();
    init_palette(); /* 设置调色板 */   
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, LIGHT_DARK_BLUE);

    sprintf(logo, "%s", "LinOS");
    logox = (binfo->scrnx - strlen(logo) * 8 * psize) / 2;
    logoy = (binfo->scrny - 30 - 16 * psize) / 2;
    show_logo8(binfo->vram, binfo->scrnx, logox, logoy, WHITE, LIGHT_DARK_BLUE, logo, psize);

    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);

    // sprintf(s, "scrnx = %d", binfo->scrnx);
    // putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, WHITE, s);

    for (;;)
    {
        io_hlt();
    }
}
