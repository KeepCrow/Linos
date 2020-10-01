void io_hlt (void);
void write_mem8 (int addr, int data);

/* 暂时无法修改为LinMain()，可惜可惜 */
void HariMain(void)
{
    int i;
    for (i = 0xa0000; i <= 0xaffff; i++)
    {
        write_mem8(i, 15);
    }

    for (;;)
    {
        io_hlt();
    }
}