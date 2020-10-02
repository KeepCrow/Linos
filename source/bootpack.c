#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

void io_hlt(void);
void io_cli(void);
void io_out8(int add, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

/* 调色板 */
void set_palette(int start, int end, unsigned char *rgb);
void init_palette(void);

/* 图形绘制函数：绘制一个长方形 */
void boxfill8(unsigned char *vram, int xsize, unsigned char c, 
                int x0, int y0, int xlen, int ylen);

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    char *p;
    init_palette(); /* 设置调色板 */

    p = (char *)0xa0000;    /* 显存初始地址 */
    boxfill8(p, 320, COL8_FF0000, 20, 20, 100, 100);
    boxfill8(p, 320, COL8_00FF00, 70, 50, 100, 100);
    boxfill8(p, 320, COL8_0000FF, 120, 80, 100, 100);

    for (;;)
    {
        io_hlt();
    }
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

void boxfill8(unsigned char *vram, int xsize, unsigned char c, 
                int x0, int y0, int xlen, int ylen)
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
