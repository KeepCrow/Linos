#include <stdio.h>
#include "naskfunc.h"
#include "graphic.h"

#define TASK_HEIGHT     28

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
 * @param[in]  x0     色块左上角水平座标
 * @param[in]  y0     色块左上角垂直座标
 * @param[in]  xlen   色块水平长
 * @param[in]  ylen   色块垂直长
 * @param[in]  lc     线条颜色
 * @param[in]  bc     填充颜色
 */
void boxfill8(char *buf, int xsize, int x0, int y0, int xlen, int ylen, char lc, char bc)
{
    int x, y;
    for (y = y0; y < y0 + ylen; y++)
    {
        for (x = x0; x < x0 + xlen; x++)
        {
            if (x == x0 || y == y0 || x == x0 + xlen - 1 || y == y0 + ylen - 1)
                buf[y * xsize + x] = lc;
            else
                buf[y * xsize + x] = bc;
        }
    }
    return;
}

// /**
//  * @brief      将一个左上角地址为(x0, y0)，长宽分别为xlen和ylen的矩形用颜色为c的线圈出来
//  *
//  * @param      vram   显存首地址
//  * @param[in]  xsize  屏幕水平像素数量
//  * @param[in]  c      线使用的颜色
//  * @param[in]  x0     色块左上角水平座标
//  * @param[in]  y0     色块左上角垂直座标
//  * @param[in]  xlen   色块水平长
//  * @param[in]  ylen   色块垂直长
//  */
// void boxhollow8(char *buf, int xsize, unsigned char c, int x0, int y0, int xlen, int ylen)
// {
//     boxfill8(buf, xsize, c, x0,        y0,            xlen, 1);
//     boxfill8(buf, xsize, c, x0,        y0,            1,    ylen);
//     boxfill8(buf, xsize, c, x0,        y0 + ylen - 1, xlen, 1);
//     boxfill8(buf, xsize, c, x0 + xlen, y0,    1,      ylen);
// }

void putfont8(char *buf, int xsize, int x, int y, char c, char *font)
{
    int i;
    char d;     /* data */
    char *p;

    for (i = 0; i < FONT_HEIGHT; i++)
    {
        p = buf + (y + i) * xsize + x;
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
void putfonts8_asc(char *buf, int xsize, int x, int y, char c, unsigned char *s)
{
    extern char hankaku[4096];
    for (; *s != 0x00; s++)
    {
        putfont8(buf, xsize, x, y, c, hankaku + *s * 16);
        x += FONT_WIDTH + FONT_MARGIN_X;
        
        if (x > xsize - FONT_WIDTH)
        {
            x = 0;
            y += LINE_SPAN;
        }
    }
    return;
}

/**
 * @brief 将一个block放到屏幕上.
 * @param vram 显存地址
 * @param vxsize 屏幕的宽度
 * @param pxsize 
 */
void putblock8_8(char *buf, int vxsize, int pxsize, int pysize, int px0, int py0, char *block_buf, int bxsize)
{
    int x, y;
    for (y = 0; y < pysize; y++)
    {
        for (x = 0; x < pxsize; x++)
        {
            buf[(py0 + y) * vxsize + (px0 + x)] = block_buf[y * bxsize + x];
        }
    }
    return;
}

void putpixle8(char *buf, int xsize, int x, int y, char c, char bc, int pixle_size)
{
    boxfill8(buf, xsize, x,     y,     pixle_size,     1,              bc, bc);
    boxfill8(buf, xsize, x,     y + 1, 1,              pixle_size,     bc, bc);
    boxfill8(buf, xsize, x + 1, y + 1, pixle_size - 1, pixle_size - 1, c,  c);
}

void putfont8_giant(char *buf, int xsize, int x, int y, char c, char bc, char *font, int psize)
{
    int i;
    char d;     /* data */

    for (i = 0; i < FONT_HEIGHT; i++)
    {
        d = font[i];
        y += psize;

        if ((d & 0x80) != 0)
            putpixle8(buf, xsize, x + 0 * psize, y, c, bc, psize);
        if ((d & 0x40) != 0)
            putpixle8(buf, xsize, x + 1 * psize, y, c, bc, psize);
        if ((d & 0x20) != 0)
            putpixle8(buf, xsize, x + 2 * psize, y, c, bc, psize);
        if ((d & 0x10) != 0)
            putpixle8(buf, xsize, x + 3 * psize, y, c, bc, psize);
        if ((d & 0x08) != 0)
            putpixle8(buf, xsize, x + 4 * psize, y, c, bc, psize);
        if ((d & 0x04) != 0)
            putpixle8(buf, xsize, x + 5 * psize, y, c, bc, psize);
        if ((d & 0x02) != 0)
            putpixle8(buf, xsize, x + 6 * psize, y, c, bc, psize);
        if ((d & 0x01) != 0)
            putpixle8(buf, xsize, x + 7 * psize, y, c, bc, psize);
    }

    return;
}

void show_logo8(char *buf, int xsize, int x, int y, char c, char bc, unsigned char *logo, int psize)
{
    extern char hankaku[4096];
    for (; *logo != 0x00; logo++)
    {
        putfont8_giant(buf, xsize, x, y, c, bc, hankaku + *logo * 16, psize);
        x += FONT_WIDTH * psize;
    }
    return;
}

void line_show8(struct SHEET *sht, enum LineNum line_num, unsigned char *msg)
{
    int xsize = sht->bxsize;
    int y = line_num * LINE_SPAN;

    boxfill8(sht->buf, xsize, 0, y, xsize, LINE_SPAN, BACK_COLOR, BACK_COLOR);
    putfonts8_asc(sht->buf, xsize, 1, y + FONT_MARGIN_Y, WHITE, msg);
    sheet_refresh(sht, 1, y + FONT_MARGIN_Y, xsize, y + LINE_SPAN);
    return;
}

static int cursor_col = 0, input_line = 0;
void line_input8(struct SHEET *sht, enum LineNum line_num, unsigned char *msg)
{
    int x, y, max_col;
    max_col = sht->bxsize / FONT_WIDTH - 1;
    x = cursor_col * FONT_WIDTH + 1;
    y = ((input_line > line_num) ? input_line : line_num) * LINE_SPAN;

    putfonts8_asc(sht->buf, sht->bxsize, x, y, WHITE, msg);
    sheet_refresh(sht, x, y, x + FONT_WIDTH, y + LINE_SPAN);
    
    cursor_col += 1;
    if (cursor_col >= max_col)
    {
        cursor_col = 0;
        input_line += 1;
    }
}

/**
 * @brief      初始化桌面背景
 *
 * @param      vram   显存首地址
 * @param[in]  xsize  屏幕水平像素数量
 * @param[in]  ysize  屏幕垂直像素数量
 */
void init_screen8(char *buf, int xsize, int ysize)
{
    boxfill8(buf, xsize, 0, 0, xsize, ysize - TASK_HEIGHT, BACK_COLOR, BACK_COLOR);
    boxfill8(buf, xsize, 0, ysize - TASK_HEIGHT, xsize, TASK_HEIGHT, BRIGHT_GRAY, BRIGHT_GRAY);
    boxfill8(buf, xsize, 0, ysize - TASK_HEIGHT, xsize, 1, WHITE, WHITE);
    boxfill8(buf, xsize, 0, ysize - TASK_HEIGHT, 57, TASK_HEIGHT, WHITE, BRIGHT_GRAY);
    return;
}
