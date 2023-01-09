#include <stdio.h>
struct BOOTINFO
{
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short screen_width;
    short screen_height;
    char *video_ram; // 4字节
};

// 8字节
struct SEGMENT_DESCRIPTOR
{
    short limit_low;
    short base_low;
    char base_mid;
    char access_right;
    char limit_high;
    char base_high;
};

// 8字节
struct GATE_DESCRIPTOR
{
    short offset_low;
    short selector;
    char dw_count;
    char access_right;
    short offset_high;
};

void io_hlt(void);
void io_cli(void);
void write_mem8(int addr, int data);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *video_ram, int screen_width, unsigned char col8, int x0, int y0, int x1, int y1);
void init_screen(struct BOOTINFO *binfo);
void putfont8(char *video_ram, int screen_width, int x, int y, char col8, char *font);
// 显示ascii str
void putfont8_ascstr(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
                 int pysize, int px0, int py0, char *buf, int bxsize);
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

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

void debug_int(struct BOOTINFO *binfo, int x, int y, unsigned char *var_name, int val)
{
    char buf[40];
    sprintf(buf, "%s = %d", var_name, val);
    putfont8_ascstr(binfo->video_ram, binfo->screen_width, x, y, COL8_RED, buf);
}
void debug_str(struct BOOTINFO *binfo, int x, int y, unsigned char *var_name, char *val)
{
    char buf[40];
    sprintf(buf, "%s = %s", var_name, val);
    putfont8_ascstr(binfo->video_ram, binfo->screen_width, x, y, COL8_RED, buf);
}

void HariMain(void)
{
    struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;
    char s[40], mcursor[256];
    int mx, my;

    init_gdtidt();
    init_palette();
    init_screen(binfo);
    mx = (binfo->screen_width - 16) / 2; /* �ｿｽ�ｿｽﾊ抵ｿｽ�ｿｽ�ｿｽ�ｿｽﾉなゑｿｽ謔､�ｿｽﾉ搾ｿｽ�ｿｽW�ｿｽv�ｿｽZ */
    my = (binfo->screen_height - 28 - 16) / 2;
    init_mouse_cursor8(mcursor, COL8_008484);
    putblock8_8(binfo->video_ram, binfo->screen_width, 16, 16, mx, my, mcursor, 16);
    sprintf(s, "(%d, %d)", mx, my);
    putfont8_ascstr(binfo->video_ram, binfo->screen_width, 0, 0, COL8_FFFFFF, s);

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

// 初始化屏幕
void init_screen(struct BOOTINFO *binfo)
{
    char *video_ram = binfo->video_ram;
    int width = binfo->screen_width;
    int height = binfo->screen_height;
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

void putfont8_ascstr(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
    extern char hankaku[4096]; // 外部字体数据
    for (; *s != '\0'; s++)
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

// gdt (global segment descriptor table)全局段号记录表
// idt (interrupt descriptor table)中断记录表
void init_gdtidt(void)
{
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)0x00270000; // 随意找空白
    struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)0x0026f800;       // 地址是随意找的空白地址
    int i;

    // GDT初始化
    // 将 段号 放在16位的（但是低3位不能用）寄存器里
    // 记录 段号 和 段 的对应关系
    // 一共可以储存8192个段 0-8191
    for (i = 0; i < 8192; i++)
    {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
    // 通过汇编语言设置gdt
    load_gdtr(0xffff, 0x0027000);

    // IDT初始化
    // 记录 中断号码 和 调用函数 的对应关系
    // 256个0-255
    for (i = 0; i < 256; i++)
    {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    // 通过汇编语言设置idt
    load_idtr(0x7ff, 0x0026f800);
}
//
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
    if (limit > 0xfffff)
    {
        ar |= 0x8000;
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
}
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
}