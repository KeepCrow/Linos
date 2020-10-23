#include <stdio.h>
#include "bootpack.h"

void set_palette(int start, int end, unsigned char *rgb)
{
    int i, eflags;

    eflags = io_load_eflags();
    io_cli();
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++)
    {
        /* 此处除以4是为了将颜色调深/浅一些，具体什么原因不知 */
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags);
    return;
}


void init_palette(void)
{
    static unsigned char table_rgb[16 * 3] = 
    {
        0x00, 0x00, 0x00,   /* 0: 黑 */
        0xff, 0x00, 0x00,   /* 1:亮红 */
        0x00, 0xff, 0x00,   /* 2:亮绿 */
        0xff, 0xff, 0x00,   /* 3:亮黄 */
        0x00, 0x00, 0xff,   /* 4:亮蓝 */
        0xff, 0x00, 0xff,   /* 5:亮紫 */
        0x00, 0xff, 0xff,   /* 6:浅亮蓝 */
        0xff, 0xff, 0xff,   /* 7:白 */
        0xc6, 0xc6, 0xc6,   /* 8:亮灰 */
        0x84, 0x00, 0x00,   /* 9:暗红 */
        0x00, 0x84, 0x00,   /* 10:暗绿 */
        0x84, 0x84, 0x00,   /* 11:暗黄 */
        0x00, 0x00, 0x84,   /* 12:暗青 */
        0x84, 0x00, 0x84,   /* 13:暗紫 */
        0x00, 0x84, 0x84,   /* 14:浅暗蓝 */
        0x84, 0x84, 0x84    /* 15:暗灰 */
    };
    set_palette(0, 15, table_rgb);
    return;
}

/**
 * @brief      将一个左上角地址为(x0, y0)，长宽分别为xlen和ylen的块染成颜色c
 *
 * @param      vram   显存首地址
 * @param[in]  xsize  屏幕水平像素数量
 * @param[in]  c      待使用的颜色
 * @param[in]  x0     色块左上角水平座标
 * @param[in]  y0     色块左上角垂直座标
 * @param[in]  xlen   色块水平长
 * @param[in]  ylen   色块垂直长
 */
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0, int xlen, int ylen)
{
    int x, y;
    for (y = y0; y < y0 + ylen; y++)
    {
        for (x = x0; x < x0 + xlen; x++)
        {
            vram[y * xsize + x] = c;
        }
    }
    return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
    int i;
    char d;     /* data */
    char *p;

    for (i = 0; i < 16; i++)
    {
        p = vram + (y + i) * xsize + x;
        d = font[i];

        if ((d & 0x80) != 0) p[0] = c;
        if ((d & 0x40) != 0) p[1] = c;
        if ((d & 0x20) != 0) p[2] = c;
        if ((d & 0x10) != 0) p[3] = c;
        if ((d & 0x08) != 0) p[4] = c;
        if ((d & 0x04) != 0) p[5] = c;
        if ((d & 0x02) != 0) p[6] = c;
        if ((d & 0x01) != 0) p[7] = c;
    }

    return;
}

/**
 * @brief      输出一串字符串，其座标为(x, y)，颜色为c
 *
 * @param      vram   显存首地址
 * @param[in]  xsize  水平像素数量
 * @param[in]  x      字符串起始水平座标
 * @param[in]  y      字符串起始垂直座标
 * @param[in]  c      字符串颜色
 * @param      s      字符串指针
 */
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
    extern char hankaku[4096];
    for (; *s != 0x00; s++)
    {
        putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
        x += 8;
    }
    return;
}

void init_mouse_cursor8(char *mouse, char bc)
{
    static char cursor[16][16] = {
        "**************..",
        "*OOOOOOOOOOO*...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
        ".............***"
    };
    int x, y;

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 16; x++)
        {
            switch (cursor[y][x])
            {
            case '*':
                mouse[y * 16 + x] = BLACK;
                break;
            case 'O':
                mouse[y * 16 + x] = WHITE; 
                break;
            case '.':
                mouse[y * 16 + x] = bc;
                break;
            default:
                break;
            }
        }
    }
}

/**
 * @brief 将一个block放到屏幕上.
 * @param vram 显存地址
 * @param vxsize 屏幕的宽度
 * @param pxsize 
 */
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize)
{
    int x, y;
    for (y = 0; y < pysize; y++)
    {
        for (x = 0; x < pxsize; x++)
        {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}

void putpixle8(char *vram, int xsize, int x, int y, char c, char bc, int pixle_size)
{
    boxfill8(vram, xsize, bc, x,     y,     pixle_size,     1);
    boxfill8(vram, xsize, bc, x,     y + 1, 1,              pixle_size);
    boxfill8(vram, xsize, c,    x + 1, y + 1, pixle_size - 1, pixle_size - 1);
}

void putfont8_giant(char *vram, int xsize, int x, int y, char c, char bc, char *font, int psize)
{
    int i;
    char d;     /* data */

    for (i = 0; i < 16; i++)
    {
        d = font[i];
        y += psize;

        if ((d & 0x80) != 0)
            putpixle8(vram, xsize, x + 0 * psize, y, c, bc, psize);
        if ((d & 0x40) != 0)
            putpixle8(vram, xsize, x + 1 * psize, y, c, bc, psize);
        if ((d & 0x20) != 0)
            putpixle8(vram, xsize, x + 2 * psize, y, c, bc, psize);
        if ((d & 0x10) != 0)
            putpixle8(vram, xsize, x + 3 * psize, y, c, bc, psize);
        if ((d & 0x08) != 0)
            putpixle8(vram, xsize, x + 4 * psize, y, c, bc, psize);
        if ((d & 0x04) != 0)
            putpixle8(vram, xsize, x + 5 * psize, y, c, bc, psize);
        if ((d & 0x02) != 0)
            putpixle8(vram, xsize, x + 6 * psize, y, c, bc, psize);
        if ((d & 0x01) != 0)
            putpixle8(vram, xsize, x + 7 * psize, y, c, bc, psize);
    }

    return;
}

void show_logo8(char *vram, int xsize, int x, int y, char c, char bc, unsigned char *logo, int psize)
{
    extern char hankaku[4096];
    for (; *logo != 0x00; logo++)
    {
        putfont8_giant(vram, xsize, x, y, c, bc, hankaku + *logo * 16, psize);
        x += 8 * psize;
    }
    return;
}

void show_line8(char *vram, int xsize, enum LineNum line_num, unsigned char *msg)
{
    int y = line_num * 17;
    boxfill8(vram, xsize, BLACK, 0, y, xsize, 17);
    putfonts8_asc(vram, xsize, 0, y + 1, DARK_GRAY, msg);
    return;
}

/**
 * @brief      初始化桌面背景
 *
 * @param      vram   显存首地址
 * @param[in]  xsize  屏幕水平像素数量
 * @param[in]  ysize  屏幕垂直像素数量
 */
void init_screen8(char *vram, int xsize, int ysize)
{
    boxfill8(vram, xsize, BACK_COLOR, 0, 0, xsize, ysize - 28);
    boxfill8(vram, xsize, BRIGHT_GRAY, 0, ysize - 28, xsize, 1);
    boxfill8(vram, xsize, WHITE, 0, ysize - 27, xsize, 1);
    boxfill8(vram, xsize, BRIGHT_GRAY, 0, ysize - 26, xsize, 26);

    boxfill8(vram, xsize, WHITE, 3, ysize - 24, 57, 1);
    boxfill8(vram, xsize, WHITE, 2, ysize - 24, 1, 21);
    boxfill8(vram, xsize, DARK_GRAY, 3, ysize - 4, 57, 1);
    boxfill8(vram, xsize, DARK_GRAY, 59, ysize - 23, 1, 19);
    boxfill8(vram, xsize, BLACK, 2, ysize - 3, 58, 1);
    boxfill8(vram, xsize, BLACK, 60, ysize - 24, 1, 22);

    boxfill8(vram, xsize, DARK_GRAY, xsize - 47, ysize - 24, 44, 1);
    boxfill8(vram, xsize, DARK_GRAY, xsize - 47, ysize - 23, 1, 20);
    boxfill8(vram, xsize, WHITE, xsize - 47, ysize - 3, 44, 1);
    boxfill8(vram, xsize, WHITE, xsize - 3, ysize - 24, 1, 22);

    return;
}
