void io_hlt(void);
void io_cli(void);
void write_mem8(int addr, int data);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

// rgb颜色表 一维数组
unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00, /*  0:黑 */
    0xff, 0x00, 0x00, /*  1:亮红 */
    0x00, 0xff, 0x00, /*  2:亮绿  */
    0xff, 0xff, 0x00, /*  3:亮黄  */
    0x00, 0x00, 0xff, /*  4:亮蓝  */
    0xff, 0x00, 0xff, /*  5:亮紫  */
    0x00, 0xff, 0xff, /*  6:浅亮蓝  */
    0xff, 0xff, 0xff, /*  7:白 */
    0xc6, 0xc6, 0xc6, /*  8:亮灰 */
    0x84, 0x00, 0x00, /*  9:暗红*/
    0x00, 0x84, 0x00, /* 10:暗绿 */
    0x84, 0x84, 0x00, /* 11:暗黄 */
    0x00, 0x00, 0x84, /* 12:暗青 */
    0x84, 0x00, 0x84, /* 13:暗紫 */
    0x00, 0x84, 0x84, /* 14:浅暗蓝 */
    0x84, 0x84, 0x84  /* 15:暗灰 */
};

void HariMain(void)
{
    int i;
    char *p;
    p = (char *)0xa0000;
    for (i = 0x0000; i <= 0xffff; i++)
    {
        // 使用8位颜色模式，可以显示2^8个颜色
        // write_mem8(i, 10); // 写入黄色
        // 用指针代替上面函数
        p[i] = i & 0x0f;
    }
    while (1)
    {
        io_hlt();
    }
}

void init_palette(void)
{
    set_palette(0, 15, table_rgb);
}

void set_palette(int start, int end, unsigned char *rgb)
{

    int i, eflags;
    eflags = io_load_eflags(); // 记录中断许可标志
    io_cli();                  // 将中断许可标志置为0，禁止中断
    io_out8(0x03c8, start);
    for (i = start; i <= end; i++)
    {
        io_out8(0x03c9, rgb[0] / 4);
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags); // 复原中断许可标志
}
