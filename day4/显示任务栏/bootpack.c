void io_hlt(void);
void io_cli(void);
void write_mem8(int addr, int data);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *video_ram, int width, unsigned char col8, int x0, int y0, int x1, int y1);
void init_screen(char *video_ram, int width, int height);

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

#define COL8_BLACK 0       // 黑色
#define COL8_RED 1         // 纯红
#define COL8_LIGHTGREEN 2  // 亮绿Lime
#define COL8_YELLOW 3      // 纯黄
#define COL8_BLUE 4        // 纯蓝
#define COL8_LIGHTPURPLE 5 // 洋红色
#define COL8_CYAN 6        // 浅亮蓝cyan
#define COL8_WHITE 7       // 白色
#define COL8_LIGHTGREY 8   // 亮灰 DarkGray
#define COL8_DARKRED 9     // 暗红
#define COL8_DARKGREEN 10  // 暗绿
#define COL8_DARKYELLOW 11 // 暗黄
#define COL8_DARKBLUE 12   // 暗蓝
#define COL8_DARKPURPLE 13 // 暗紫
#define COL8_DARKCYAN 14   // 浅暗蓝
#define COL8_DARKGREY 15   // 暗灰

void HariMain(void)
{
    char *video_ram;
    int width, height;
    video_ram = (char *)0xa0000;
    width = 320;
    height = 200;
    init_palette();
    boxfill8(video_ram, width, COL8_008484, 0, 0, width - 1, height - 29);
    boxfill8(video_ram, width, COL8_C6C6C6, 0, height - 28, width - 1, height - 28);
    boxfill8(video_ram, width, COL8_FFFFFF, 0, height - 27, width - 1, height - 27);
    boxfill8(video_ram, width, COL8_C6C6C6, 0, height - 26, width - 1, height - 1);

    boxfill8(video_ram, width, COL8_FFFFFF, 3, height - 24, 59, height - 24);
    boxfill8(video_ram, width, COL8_FFFFFF, 2, height - 24, 2, height - 4);
    boxfill8(video_ram, width, COL8_848484, 3, height - 4, 59, height - 4);
    boxfill8(video_ram, width, COL8_848484, 59, height - 23, 59, height - 5);
    boxfill8(video_ram, width, COL8_000000, 2, height - 3, 59, height - 3);
    boxfill8(video_ram, width, COL8_000000, 60, height - 24, 60, height - 3);

    boxfill8(video_ram, width, COL8_848484, width - 47, height - 24, width - 4, height - 24);
    boxfill8(video_ram, width, COL8_848484, width - 47, height - 23, width - 47, height - 4);
    boxfill8(video_ram, width, COL8_FFFFFF, width - 47, height - 3, width - 4, height - 3);
    boxfill8(video_ram, width, COL8_FFFFFF, width - 3, height - 24, width - 3, height - 3);
    while (1)
    {
        io_hlt();
    }
}

// 初始化调色板
void init_palette(void)
{
    // rgb颜色表 一维数组
    static unsigned char table_rgb[16 * 3] = {
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

// 绘制矩形 改写
void boxfill8(unsigned char *video_ram, int screen_width, unsigned char col8, int x0, int y0, int x1, int y1)
{
    int x, y;
    for (y = y0; y <= y1; y++)
    {
        for (x = x0; x <= x1; x++)
            video_ram[y * screen_width + x] = col8;
    }
}
