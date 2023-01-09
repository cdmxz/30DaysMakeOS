/* asmhead.nas */
#define ADR_BOOTINFO 0x00000ff0
// 地址0x0ff0-0x0fff
// struct BOOTINFO
// {
//     char cyls;  // 启动区读硬盘读到何处为止
//     char leds;  // 启动时键盘LED状态
//     char vmode; // 显卡模式 多少位彩色
//     char reserve;
//     short screen_width; // 分辨率
//     short screen_height;
//     char *video_ram;
// };
struct BOOTINFO
{               /* 0x0ff0-0x0fff */
    char cyls;  /* 僽乕僩僙僋僞偼偳偙傑偱僨傿僗僋傪撉傫偩偺偐 */
    char leds;  /* 僽乕僩帪偺僉乕儃乕僪偺LED偺忬懺 */
    char vmode; /* 價僨僆儌乕僪  壗價僢僩僇儔乕偐 */
    char reserve;
    short scrnx, scrny; /* 夋柺夝憸搙 */
    char *vram;
};

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
// void write_mem8(int addr, int data);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *video_ram, int screen_width, unsigned char col8, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *video_ram, int screen_width, int x, int y, char col8, char *font);
// 显示ascii str
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
                 int pysize, int px0, int py0, char *buf, int bxsize);

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

/* dsctbl.c */
// 8字节
struct SEGMENT_DESCRIPTOR
{
    // base表示段的基地址(32位)，这里分成三个变量表示（为了与80286时代的程序兼容）
    short base_low;
    char base_mid;
    char base_high;
    // limit表示段上限（段的字节大小）
    // 段上限只能占20位（limit_low的16位+limit_high的低4位）
    // limit_high的高4位存放段属性
    // 因为段上限只能占20位，最大只能表示1MB，所以要设置段属性的Gbit为1，
    // 告诉cpu将limit的单位解释为page（1page=4kb）,设置0则解释为byte
    // 这样就能表示4G内存
    short limit_low;
    char limit_high;
    // 段属性（12位）高4位在limit_high里，称为扩展访问权
    // 这4位由“GD00”构成，G是GBit，D是段模式，=1（表示32位模式），=0（16位）
    // 低8位在access_right
    // access_right的值
    // 未使用：0x00
    // 系统用：0x92 不可执行可读写 0x9a 可执行可读不可写
    // 应用程序用：0xf2  不可执行可读写  0xfa 可执行可读不可写
    char access_right;
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
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e
/* dsctbl.c */

/* int.c */
void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1
/* int.c */
