#include "window.h"
#include "graphic.h"

#define TITLE_HEIGHT 20

void make_window8(unsigned char *buf, int xsize, int ysize, char *title)
{
    char c;
    int x, y;
    static char closebtn[14][16] = {
        "@@@@@@@@@@@@@@@@",
        "@QQQQQQQQQQQQQQ@",
        "@QQ**QQQQQQ**QQ@",
        "@QQQ**QQQQ**QQQ@",
        "@QQQQ**QQ**QQQQ@",
        "@QQQQQ****QQQQQ@",
        "@QQQQQQ**QQQQQQ@",
        "@QQQQQ****QQQQQ@",
        "@QQQQ**QQ**QQQQ@",
        "@QQQ**QQQQ**QQQ@",
        "@QQ**QQQQQQ**QQ@",
        "@QQQQQQQQQQQQQQ@",
        "@QQQQQQQQQQQQQQ@",
        "@@@@@@@@@@@@@@@@"
    };

    boxfill8(buf, xsize, 0, 0, xsize, ysize, WHITE, BRIGHT_GRAY);
    boxfill8(buf, xsize, 1, 1, xsize - 2, TITLE_HEIGHT, DARK_BLUE, DARK_BLUE);
    putfonts8_asc(buf, xsize, 24, 4, WHITE, title);

    for (y = 0; y < 14; y++)
    {
        for (x = 0; x < 16; x++)
        {
            switch (closebtn[y][x])
            {
            case '@':
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
            buf[(4 + y) * xsize + (xsize - 21 + x)] = c;
        }
    }
    return;
}
