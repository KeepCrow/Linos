#include <stdio.h>

#define BLACK           0   /* 000000 */
#define BRIGHT_RED      1   /* FF0000 */
#define BRIGHT_GREEN    2   /* 00FF00 */
#define BRIGHT_YELLOW   3   /* FFFF00 */
#define BRIGHT_BLUE     4   /* 0000FF */
#define BRIGHT_PURPLE   5   /* FF00FF */
#define LIGHT_BLUE      6   /* 00FFFF */
#define WHITE           7   /* FFFFFF */
#define BRIGHT_GRAY     8   /* C6C6C6 */
#define DARK_RED        9   /* 840000 */
#define DARK_GREEN      10  /* 008400 */
#define DARK_YELLOW     11  /* 848400 */
#define DARK_BLUE       12  /* 000084 */
#define DARK_PURPLE     13  /* 840084 */
#define LIGHT_DARK_BLUE 14  /* 008484 */
#define DARK_GRAY       15  /* 848484 */

/* 调色板 */
void init_palette(void);
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize,
                    int px0, int py0, char *buf, int bxsize);

void init_screen(char *vram, int xsize, int ysize); 
void init_mouse_cursor8(char *mouse, char bc);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

struct BOOTINFO
{
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};

void init_gdtidt(void);

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
