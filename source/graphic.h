#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

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
#define BACK_COLOR      BLACK
#define INVI_COLOR      99

#define LINE_SPAN       20  /* 行高 */
#define MAX_LINE        10
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
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0, int xlen, int ylen);
void init_screen8(char *vram, int xsize, int ysize); 
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
void init_mouse_cursor8(char *mouse, char bc);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void show_logo8(char *vram, int xsize, int x, int y, char c, char bc, unsigned char *logo, int psize);
void show_line8(char *vram, int xsize, enum LineNum line_num, unsigned char *msg);

#endif
