#include "window.h"
#include "graphic.h"

void inputwin_init(struct INPUT_WIN *win, unsigned char *buf, int xsize, int ysize, char *title)
{
    win->base.buf     = buf;
    win->base.title   = title;
    win->base.xsize   = xsize;
    win->base.ysize   = ysize;
    win->input_x = WIN_INPUT_X0;
    win->input_y = WIN_INPUT_Y0;
}

static inline void win_make_title(struct WINDOW *win, char title_clr, char font_clr)
{
    char c;
    int x, y;
    int close_x   = win->xsize - WIN_MARGIN_X - 16; /* closebtn 水平方向是16个像素 */
    int close_y   = (TITLE_HEIGHT + WIN_MARGIN_Y - FONT_HEIGHT) / 2;

    static char closebtn[16][16] = {
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQ**QQQQ**QQQQ",
        "QQQQQ**QQ**QQQQQ",
        "QQQQQQ****QQQQQQ",
        "QQQQQQQ**QQQQQQQ",
        "QQQQQQ****QQQQQQ",
        "QQQQQ**QQ**QQQQQ",
        "QQQQ**QQQQ**QQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
    };

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 16; x++)
        {
            switch (closebtn[y][x])
            {
            case 'Q':
                c = title_clr;
                break;
            case '*':
                c = font_clr;
                break;
            }
            win->buf[(close_y + y) * win->xsize + (close_x + x)] = c;
        }
    }
}

void inputwin_make(struct INPUT_WIN *win, char line_clr, char title_clr, char body_clr, char font_clr)
{
    unsigned char *buf = win->base.buf;
    int xsize     = win->base.xsize;
    int ysize     = win->base.ysize;
    int title_x0  = WIN_MARGIN_X * 2;
    int title_y0  = (TITLE_HEIGHT + WIN_MARGIN_Y - FONT_HEIGHT) / 2;
    int body_x0   = WIN_MARGIN_X;
    int body_y0   = TITLE_HEIGHT + WIN_MARGIN_Y;
    int body_xlen = xsize - 2 * WIN_MARGIN_X;
    int body_ylen = ysize - 2 * WIN_MARGIN_Y - TITLE_HEIGHT;

    boxfill8(buf, xsize, 0, 0, xsize, ysize, line_clr, title_clr);
    boxfill8(buf, xsize, body_x0, body_y0, body_xlen, body_ylen, line_clr, body_clr);
    putfonts8_asc(buf, xsize, title_x0, title_y0, font_clr, win->base.title);
    win_make_title(&(win->base), title_clr, font_clr);
    return;
}

static inline void input_advance(struct INPUT_WIN *win)
{
    win->input_x += FONT_MARGIN_X + FONT_WIDTH;
    if (win->input_x + FONT_MARGIN_X + FONT_WIDTH >= win->base.xsize - WIN_MARGIN_X)
    {
        win->input_x = WIN_INPUT_X0;
        win->input_y += FONT_MARGIN_Y + FONT_HEIGHT;
    }
}

void inputwin_input(struct INPUT_WIN *win, struct SHEET *sht, unsigned char *str)
{
    int i;
    char msg[2];
    for (i = 0; str[i] != 0; i++)
    {
        msg[0] = str[i];
        boxfill8(win->base.buf, win->base.xsize, win->input_x, win->input_y, FONT_WIDTH + FONT_MARGIN_X, FONT_HEIGHT, BODY_COLOR, BODY_COLOR);
        putfonts8_asc(win->base.buf, win->base.xsize, win->input_x, win->input_y, FONT_COLOR, msg);
        sheet_refresh(sht, win->input_x, win->input_y, FONT_WIDTH + FONT_MARGIN_X, FONT_HEIGHT);
        input_advance(win);
    }
}

void inputwin_del(struct INPUT_WIN *win)
{
    int blockx = FONT_WIDTH + FONT_MARGIN_X;
    if (win->input_x - blockx >= WIN_INPUT_X0)
        boxfill8(win->base.buf, win->base.xsize, win->input_x - blockx, win->input_y, 2 * blockx, FONT_HEIGHT, BODY_COLOR, BODY_COLOR);
    win->input_x -= FONT_WIDTH + FONT_MARGIN_X;   
}

void staticwin_init(struct STATIC_WIN *win, unsigned char *buf, int xsize, int ysize, char *title)
{
    win->base.buf     = buf;
    win->base.title   = title;
    win->base.xsize   = xsize;
    win->base.ysize   = ysize;
}

void staticwin_make(struct STATIC_WIN *win, char line_clr, char title_clr, char body_clr, char font_clr, char with_title)
{
    unsigned char *buf = win->base.buf;
    int xsize     = win->base.xsize;
    int ysize     = win->base.ysize;
    int title_x0  = WIN_MARGIN_X * 2;
    int title_y0  = (TITLE_HEIGHT + WIN_MARGIN_Y - FONT_HEIGHT) / 2;
    int body_x0   = WIN_MARGIN_X;
    int body_y0   = TITLE_HEIGHT + WIN_MARGIN_Y;
    int body_xlen = xsize - 2 * WIN_MARGIN_X;
    int body_ylen = ysize - 2 * WIN_MARGIN_Y - TITLE_HEIGHT;

    if (with_title == WITH_TITLE)
    {
        boxfill8(buf, xsize, 0, 0, xsize, ysize, line_clr, title_clr);
        boxfill8(buf, xsize, body_x0, body_y0, body_xlen, body_ylen, line_clr, body_clr);
        putfonts8_asc(buf, xsize, title_x0, title_y0, font_clr, win->base.title);
        win_make_title(&(win->base), title_clr, font_clr);
    }
    else
    {
        boxfill8(buf, xsize, 0, 0, xsize, ysize, body_clr, body_clr);
    }
}