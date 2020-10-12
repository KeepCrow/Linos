#include <stdio.h>
#include "bootpack.h"

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    char s[10];
    char mouse[256];
    int mx, my;
    struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;

    init_palette(); /* 设置调色板 */   
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny); 
    init_mouse_cursor8(mouse, LIGHT_DARK_BLUE);
    
    mx = (binfo->scrnx - 16) / 2;
    my = (binfo->scrny - 28 - 16) / 2;
    putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mouse, 16);
    putfonts8_asc(binfo->vram, binfo->scrnx, 9, 9, BLACK, "Hello Linos");
    putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, WHITE, "Hello Linos");

    sprintf(s, "scrnx = %d", binfo->scrnx);
    putfonts8_asc(binfo->vram, binfo->scrnx, 8, 25, WHITE, s);

    for (;;)
    {
        io_hlt();
    }
}
