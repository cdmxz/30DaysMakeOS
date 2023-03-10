/*中断关系*/

#include "bootpack.h"

void init_pic(void)
/* PIC初始化 */
{
    io_out8(PIC0_IMR, 0xff); /* 禁止所有中断 */
    io_out8(PIC1_IMR, 0xff); /* 禁止所有中断 */

    io_out8(PIC0_ICW1, 0x11);   /* 边缘触发模式（edge trigger mode） */
    io_out8(PIC0_ICW2, 0x20);   /* IRQ0-7由INT20-27接收 */
    io_out8(PIC0_ICW3, 1 << 2); /* PIC1由IRQ2相连 */
    io_out8(PIC0_ICW4, 0x01);   /* 无缓冲区模式 */

    io_out8(PIC1_ICW1, 0x11); /* 边缘触发模式（edge trigger mode） */
    io_out8(PIC1_ICW2, 0x28); /* IRQ8-15由INT28-2f接收 */
    io_out8(PIC1_ICW3, 2);    /* PIC1由IRQ2连接 */
    io_out8(PIC1_ICW4, 0x01); /* 无缓冲区模式 */

    io_out8(PIC0_IMR, 0xfb); /* 11111011 PIC1以外全部禁止 */
    io_out8(PIC1_IMR, 0xff); /* 11111111 禁止所有中断 */

    return;
}

void inthandler27(int *esp)
/*PIC0的不完全中断对策
Athlon64X2机等由于芯片组的原因，在PIC初始化时仅发生一次该中断
该中断处理函数对该中断什么都不做就过去
为什么什么都不用做？
→该中断是由于PIC初始化时的电噪声而产生的
*/
{
    io_out8(PIC0_OCW2, 0x67); /*向PIC通知IRQ-07受理完成（参照7-1）*/
    return;
}
