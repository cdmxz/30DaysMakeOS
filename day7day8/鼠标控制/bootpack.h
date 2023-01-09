#include <stdio.h>

#define arrcount(array) (sizeof(array) / sizeof(array[0]))
/* asmhead.nas */
struct BOOTINFO
{ /* 0x0ff0-0x0fff */
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short scrnx, scrny;
    char *vram;
};
#define ADR_BOOTINFO 0x00000ff0

/* naskfunc.nas */
// 休眠（处理器进入暂停状态，不执行任何操作）
void io_hlt(void);
// 屏蔽中断
void io_cli(void);
// 开放中断
void io_sti(void);
// 开放中断然后休眠
void io_stihlt(void);
// 寄存器从外部设备读取数据
int io_in8(int port);
// 寄存器写入数据到外部设备
void io_out8(int port, int data);
//
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
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
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
struct SEGMENT_DESCRIPTOR
{
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};
struct GATE_DESCRIPTOR
{
    short offset_low, selector;
    char dw_count, access_right;
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

/* int.c */
// 键盘中断缓冲区
#define PORT_KEYDAT 0x0060
void init_pic(void);
void inthandler27(int *esp);
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

// fifo.c
struct FIFO8
{
    unsigned char *buf; // 缓冲区的地址
    int p;              // 表下一个数据写入地址
    int q;              // 下一个数据读出地址
    int size;           // 缓冲区的总字节数
    int free;           // 缓冲区里没有数据的字节数
    int flags;          // 记录是否溢出 -1有溢出 0没有溢出
};
#define FLAGS_OVERRUN 0x0001
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

// mouse.c
#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
struct MOUSE_DEC
{ // 每次从鼠标那里送过来的数据都应该是3个字节一组
    unsigned char buf[3];
    unsigned char phase; // 用来记住接收鼠标数据的工作进展到了什么阶段

    int x, y, btn;
};

void inthandler2c(int *esp);

// keyboard.c
#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(void);
