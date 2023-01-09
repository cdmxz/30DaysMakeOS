; hello-os
; TAB=4

    ORG 0x7c00  ; 指明程序的装载地址

; 以下代码用于描述FAT12格式软盘
    JMP entry
    DB  0x90
    DB  "helloipl" ; 启动区的名称任意的字符串(8字节)
    DW  512 ; 每个扇区的大小 固定512字节
    DB  1 ; 硬盘簇大小 固定1个扇区
    DW  1 ; FAT 的起始位置 一般从第一扇区开始
    DB  2 ; FAT的个数必须为2
    DW  224 ; 根目录的大小 一般为225项
    DW  2880 ; 磁盘大小 必须2880扇区
    DB  0xf0 ; 磁盘总类 固定0xf0
    DW  9 ; FAT长度
    DW  18 ; 1个磁道有几个扇区18
    DW  2 ; 磁头数，必须2
    DD  0 ; 不使用分区，必须0
    DD  2880 ; 重写一次磁盘大小
    DB  0,0,0x29 ; 意义不明，固定
    DD  0xffffffff ; 卷标号码
    DB  "Hello-os   " ;磁盘名称 11字节
    DB  "FAT12   " ; 磁盘格式名称 8字节
    RESB 18 ; 先空出18字节        

; 程序主体

entry:
        MOV AX,0    ; 初始化寄存器
        MOV SS,AX
        MOV SP,0x7c00
        MOV DS,AX
        MOV ES,AX
        MOV SI,msg

putloop:
        MOV AL,[SI]
        ADD SI,1 ;SI加1
        CMP Al,0
        JE fin
        MOV AH,0x0e ; 显示一个文字
        MOV BX,15 ; 字符颜色
        INT 0x10; 调用显卡bios
        JMP putloop

fin:
        HLT ;让cpu停止，等待指令
        JMP fin; 无限循环

msg:
        DB 0x0a,0x0a;换行2次
        DB "hello,world"
        DB 0x0a
        DB 0
        RESB 0x7dfe-$
        DB 0x55,0xaa

