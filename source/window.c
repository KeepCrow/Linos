#include "window.h"
#include "graphic.h"

void init_window8(struct WINDOW *win, unsigned char *buf, char *title, int xsize, int ysize, unsigned char tc, unsigned bc)
{
    win->buf   = buf;
    win->title = title;
    win->xsize = xsize;
    win->ysize = ysize;
    win->title_color = tc;
    win->body_color  = bc;
}

void make_window8(struct WINDOW *win)
{
    char c;
    int x, y;
    unsigned char *buf = win->buf;
    int xsize = win->xsize, ysize = win->ysize;

    static char closebtn[15][16] = {
        "QQQQQQQQQQQQQQQQ",
        "QQQQQQQQQQQQQQQQ",
        "QQ**QQQQQQQQ**QQ",
        "QQQ**QQQQQQ**QQQ",
        "QQQQ**QQQQ**QQQQ",
        "QQQQQ**QQ**QQQQQ",
        "QQQQQQ****QQQQQQ",
        "QQQQQQQ**QQQQQQQ",
        "QQQQQQ****QQQQQQ",
        "QQQQQ**QQ**QQQQQ",
        "QQQQ**QQQQ**QQQQ",
        "QQQ**QQQQQQ**QQQ",
        "QQ**QQQQQQQQ**QQ",
        "QQQQQQQQQQQQQQQQ"
        "QQQQQQQQQQQQQQQQ",
    };

    boxfill8(buf, xsize, 0, 0, xsize, ysize, WHITE, win->body_color);
    boxfill8(buf, xsize, 1, 1, xsize - 2, TITLE_HEIGHT, win->title_color, win->title_color);
    boxfill8(buf, xsize, 1, TITLE_HEIGHT + 2, xsize - 2, ysize - TITLE_HEIGHT - 3, DARK_GRAY, WHITE);
    putfonts8_asc(buf, xsize, 4, 4, WHITE, win->title);

    for (y = 0; y < 14; y++)
    {
        for (x = 0; x < 16; x++)
        {
            switch (closebtn[y][x])
            {
            case 'Q':
                c = win->title_color;
                break;
            case '*':
                c = WHITE;
                break;
            }
            buf[(4 + y) * xsize + (xsize - 21 + x)] = c;
        }
    }
    return;
}
