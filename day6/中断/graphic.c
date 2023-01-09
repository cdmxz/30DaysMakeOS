/* 图形函数 */

#include "bootpack.h"

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

// 初始化屏幕
void init_screen8(struct BOOTINFO *binfo)
{
    char *video_ram = binfo->vram;
    int width = binfo->scrnx;
    int height = binfo->scrny;
    boxfill8(video_ram, width, COL8_DARKGREEN, 0, 0, width - 1, height - 29);
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
}

void putfont8(char *video_ram, int screen_width, int x, int y, char col8, char *font)
{
    int i;
    char *p;
    char data;
    for (i = 0; i < 16; i++)
    {
        p = video_ram + (y + i) * screen_width + x;
        data = font[i];
        if ((data & 0x80) != 0)
        { // data & 0x80，如果运算结果是0，则data的最左边是0，如果非0，则为1
            p[0] = col8;
        }
        if ((data & 0x40) != 0)
        {
            p[1] = col8;
        }
        if ((data & 0x20) != 0)
        {
            p[2] = col8;
        }
        if ((data & 0x10) != 0)
        {
            p[3] = col8;
        }
        if ((data & 0x08) != 0)
        {
            p[4] = col8;
        }
        if ((data & 0x04) != 0)
        {
            p[5] = col8;
        }
        if ((data & 0x02) != 0)
        {
            p[6] = col8;
        }
        if ((data & 0x01) != 0)
        {
            p[7] = col8;
        }
    }
}

// void putfont8_ascstr(char *vram, int xsize, int x, int y, char c, unsigned char *s)
//{
//     extern char hankaku[4096]; // 外部字体数据
//     for (; *s != '\0'; s++)
//     {
//         putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
//         x += 8;
//     }
// }

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
    extern char hankaku[4096]; // 外部字体数据
    for (; *s != 0x00; s++)
    {
        putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
        x += 8;
    }
}

void init_mouse_cursor8(char *mouse, char bc)
{
    // 鼠标指针16x16
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
        ".............***"};
    int x, y;

    for (y = 0; y < 16; y++)
    {
        for (x = 0; x < 16; x++)
        {
            if (cursor[y][x] == '*')
            {
                mouse[y * 16 + x] = COL8_000000;
            }
            if (cursor[y][x] == 'O')
            {
                mouse[y * 16 + x] = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.')
            {
                mouse[y * 16 + x] = bc;
            }
        }
    }
    return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
                 int pysize, int px0, int py0, char *buf, int bxsize)
{
    int x, y;
    for (y = 0; y < pysize; y++)
    {
        for (x = 0; x < pxsize; x++)
        {
            vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
    }
    return;
}
