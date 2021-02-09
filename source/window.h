#ifndef _WINDOW_H_
#define _WINDOW_H_

/* 初始窗口大小: scrnx的5/9，scrny的2/3 */

#include "graphic.h"
#include "sheet.h"

#define LINE_COLOR          DIM_GRAY
#define BODY_COLOR          DARK_WHITE
#define TITLE_BAR_COLOR     DARK_BROWN
#define TITLE_FONT_COLOR    DARK_WHITE
#define FONT_COLOR          DARK_BROWN

#define TITLE_HEIGHT    32
#define WIN_MARGIN_X    6
#define WIN_MARGIN_Y    4
#define WIN_INPUT_X0    (WIN_MARGIN_X * 2)
#define WIN_INPUT_Y0    (WIN_MARGIN_Y + TITLE_HEIGHT)

#define WIN_XSIZE(binfo)    (binfo->scrnx * 5 / 6)
#define WIN_YSIZE(binfo)    (binfo->scrny * 2 / 3)

#define WITHOUT_TITLE   0
#define WITH_TITLE      1

struct WINDOW
{
    unsigned char *title, *buf;
    int xsize, ysize;
    struct SHEET *sht;
};

struct INPUT_WIN
{
    struct WINDOW base;
    int input_x, input_y;
};

struct STATIC_WIN
{
    struct WINDOW base;
};

void inputwin_init(struct INPUT_WIN *win, unsigned char *buf, int xsize, int ysize, char *title);
void inputwin_make(struct INPUT_WIN *win, char line_clr, char title_clr, char body_clr, char font_clr);
void inputwin_input(struct INPUT_WIN *win, struct SHEET *sht, unsigned char *str);
void inputwin_del(struct INPUT_WIN *win);

void staticwin_init(struct STATIC_WIN *win, unsigned char *buf, int xsize, int ysize, char *title);
void staticwin_make(struct STATIC_WIN *win, char line_clr, char title_clr, char body_clr, char font_clr, char with_title);

#endif
