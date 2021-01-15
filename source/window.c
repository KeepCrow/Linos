#include "window.h"

void make_window8(unsigned char *buf, int xsize, int ysize, char *title)
{
    char c;
    int x, y;
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQ**QQQQ**QQ$@",
        "OQQQQ**QQ**QQQ$@",
        "OQQQQQ****QQQQ$@",
        "OQQQQQQ**QQQQQ$@",
        "OQQQQQ****QQQQ$@",
        "OQQQQ**QQ**QQQ$@",
        "OQQQ**QQQQ**QQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@",
        "@@@@@@@@@@@@@@@@"
    };

    boxfill8(buf, xsize, BRIGHT_GRAY, 0,         0,         xsize,     1        );
    boxfill8(buf, xsize, WHITE,       1,         1,         xsize - 1, 1        );
    boxfill8(buf, xsize, BRIGHT_GRAY, 0,         0,         1,         ysize    );
    boxfill8(buf, xsize, WHITE,       1,         1,         1,         ysize - 1);
    boxfill8(buf, xsize, DARK_GRAY,   1,         ysize - 2, xsize - 2, 1        );
    boxfill8(buf, xsize, BLACK,       0,         ysize - 1, xsize,     1        );
    boxfill8(buf, xsize, DARK_GRAY,   xsize - 2, 1,         1,         ysize - 2);
    boxfill8(buf, xsize, BLACK,       xsize - 1, 0,         1,         ysize    );
    boxfill8(buf, xsize, BRIGHT_GRAY, 2,         2,         xsize - 4, ysize - 4);
    boxfill8(buf, xsize, DARK_BLUE,   3,         3,         xsize - 6, 17       );
    putfonts8_asc(buf, xsize, 24, 4, WHITE, title);

    for (y = 0; y < 14; y++)
    {
        for (x = 0; x < 16; x++)
        {
            switch (closebtn[y][x])
            {
            case '@':
                c = BLACK;
                break;
            case '$':
                c = DARK_GRAY;
                break;
            case 'Q':
                c = BRIGHT_GRAY;
                break;
            case '*':
                c = DARK_RED;
                break;
            default:
                c = WHITE;
                break;
            }
            buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
        }
    }
    return;
}
