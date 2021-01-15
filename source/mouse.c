#include "mouse.h"
#include "int.h"
#include "debug.h"

void enable_mouse(struct MOUSE_DEC *mdec)
{
    /* 激活鼠标 */
    wait_KBC_sendready();
    io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    wait_KBC_sendready();
    io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);  /* 这一句执行完毕后，鼠标会返回0xfa */
    mdec->phase = 0;
    return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
    if (mdec->phase == 0)
    {
        if (dat == 0xfa)
            mdec->phase = 1;
        return 0;
    }
    
    if (mdec->phase == 1)
    {
        if ((dat & 0xc8) == 0x08)
        {
            mdec->buf[0] = dat;
            mdec->phase = 2;
        }
        return 0;
    }

    if (mdec->phase == 2)
    {
        mdec->buf[1] = dat;
        mdec->phase = 3;
        return 0;
    }

    if (mdec->phase == 3)
    {
        mdec->buf[2] = dat;
        mdec->phase = 1;
        mdec->btn = mdec->buf[0] & 0x07;    /* 111 */
        mdec->x = mdec->buf[1];
        mdec->y = mdec->buf[2];

        /* https://www.cnblogs.com/qiweiwang/archive/2011/03/21/1990346.html
         * bit7: Y overflow     bit6: Y overflow        bit5: Y sign bit     
         * bit4: X sign bit     bit3: always 1          bit2: middle btn
         * bit1: right btn      bit0: left btn
         */
        if ((mdec->buf[0] & 0x10) != 0)
            mdec->x |= 0xffffff00;
        if ((mdec->buf[0] & 0x20) != 0)
            mdec->y |= 0xffffff00;
        mdec->y = -mdec->y; /* 鼠标的y方向与画面相反 */

        return 1;
    }

    return -1;
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
