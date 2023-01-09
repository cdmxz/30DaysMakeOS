; haribote-os
; TAB=4
CYLS EQU x0xff0 ;设定启动区
LEDS EQU x0xff1 
VMODE EQU x0xff2;关于颜色数目的信息，颜色的位数
SCRNX EQU x0xff4;分辨率x
SCRNY EQU x0xff6;分辨率y
VRAM EQU x0xff8;图像缓冲区开始的地址

    ORG 0xc200 
    ; 用16进制编辑器打开，文件名在0x002600
    ; 文件内容在0x004200
    ; 所以启动扇区加载完后跳转到0x7c00（软盘内容装载地址）+0x004200

    MOV AL,0x13; VGA显卡，320x200x8位彩色
    MOV AH,0x00
    INT 0x10;调用显卡bios函数
    ;将画面模式，分辨率保存到内存
    MOV BYTE [VMODE],8
    MOV WORD [SCRNX],320
    MOV WORD [SCRNY],200
    MOV DWORD [VRAM],x0000a0000

    ;用BIOS取得键盘上各种LED指示灯的状态
    MOV AH,0x02
    INT 0x16 ;键盘BIOS函数
    MOV [LEDS],AL

fin:
        HLT ;让cpu停止，等待指令
        JMP fin; 无限循环