#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#include "sheet.h"

#define BLACK           0   /* 000000 */
#define BRIGHT_RED      1   /* FF0000 */
#define BRIGHT_GREEN    2   /* 00FF00 */
#define BRIGHT_YELLOW   3   /* FFFF00 */
#define BRIGHT_BLUE     4   /* 0000FF */
#define DIM_GRAY        5   /* D2DFE3 */
#define LIGHT_BLUE      6   /* 00FFFF */
#define WHITE           7   /* FFFFFF */
// #define BRIGHT_GRAY     8   /* C6C6C6 */
#define BRIGHT_GRAY     8   /* E3E3E3 */
#define DARK_RED        9   /* 840000 */
#define DARK_GREEN      10  /* 008400 */
#define DARK_YELLOW     11  /* 848400 */
#define DARK_BROWN      12  /* 000084 */
#define JEANS_BLUE      13  /* 840084 */
#define DARK_WHITE      14  /* 008484 */
#define CEMENT_GRAY     15  /* 848484 */
#define BACK_COLOR      JEANS_BLUE
#define INVI_COLOR      99

#define LINE_SPAN       20  /* 行高 */
#define FONT_WIDTH      8   /* 字符宽 */ 
#define FONT_HEIGHT     16  /* 字符高 */
#define FONT_MARGIN_Y   ((LINE_SPAN - FONT_HEIGHT) / 2)
#define FONT_MARGIN_X   2

enum LineNum
{
    LN_MOUSE,
    LN_KEYBOARD,
    LN_MEM,
};

void init_palette(void);
void boxfill8(char *buf, int xsize, int x0, int y0, int xlen, int ylen, char lc, char bc);
void init_screen8(char *buf, int xsize, int ysize); 
void putblock8_8(char *buf, int vxsize, int pxsize, int pysize, int px0, int py0, char *block_buf, int bxsize);
void putfonts8_asc(char *buf, int xsize, int x, int y, char c, unsigned char *s);
void show_logo8(char *buf, int xsize, int x, int y, char c, char bc, unsigned char *logo, int psize);
void line_show8(struct SHEET *sht, enum LineNum line_num, unsigned char *msg);
void line_input8(struct SHEET *sht, enum LineNum line_num, unsigned char *msg);

#endif
