#define PALETTE8 0x03c8
#define PALETTE9 0x03c9

void io_hlt(void);
void io_cli(void);
void io_out8(int add, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

/* 调色板 */
void init_palette(void);
void set_palette(int start, int end, unsigned int *rgb);


/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    int i;
    char *p;

    p = 0xa0000;
    for (i = 0; i <= 0xffff; i++)
    {
        p[i] = i & 0x000f;
    }

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

void set_palette(int start, int end, unsigned int *rgb)
{
    int i, eflags;

    eflags = io_load_eflags();
    io_cli();
    io_out8(PALETTE8, start);
    for (i = start; i <= end; i++)
    {
        /* 此处除以4是为了将颜色调深/浅一些，具体什么原因不知 */
        io_out8(PALETTE9, rgb[0] / 4);
        io_out8(PALETTE9, rgb[1] / 4);
        io_out8(PALETTE9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags);
    return;
}
