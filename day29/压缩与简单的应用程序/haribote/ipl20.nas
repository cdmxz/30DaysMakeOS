; haribote-ipl 加载程序
; TAB=4

CYLS    EQU 20; 相等于#define cyls 20  要载入多少数据
        ORG 0x7c00  ; 指明启动区的装载地址 可以为0x7c00-0x7dff之间


; 以下代码用于描述FAT12格式软盘

        JMP entry
        DB  0x90
        DB  "HARIBOTE" ; 启动区的名称任意的字符串(8字节)
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
        DB  "HARIBOTEOS " ;磁盘名称 11字节
        DB  "FAT12   " ; 磁盘格式名称 8字节
        RESB 18 ; 先空出18字节        

; 程序主体

entry:
        MOV AX,0    ; 初始化寄存器
        MOV SS,AX
        MOV SP,0x7c00
        MOV DS,AX

; 读软盘

        MOV AX,0x0820 ; 数据区的装载地址必须在0x7dff（0x7c00-0x7dff启动区用了）之后
        MOV ES,AX
        ; 设定寄存器的值=传参
        MOV CH,0 ; 柱面0 软盘最外围的一圈
        MOV DH,0 ; 磁头0 软盘正面磁头
        MOV CL,2 ; 扇区2 一个扇区512字节
readloop:
        MOV SI,0 ; 加载记录失败次数 避免软盘抽风，偶尔读不了数据

retry:
        MOV AH,0x02 ; AH=0x02 读入磁盘
        MOV AL,1; 1个扇区
        MOV BX,0
        MOV DL,0x00;A驱动器
        INT 0x13;调用BIOS磁盘函数
        JNC next;进位标志为0跳转（成功）next 
        ADD SI,1;SI+1
        CMP SI,5; 比较SI与5
        JAE error; SI >= 5，跳转error
        MOV AH,0x00
        MOV DL,0x00
        INT 0x13; 重置驱动器
        JMP retry
next:
; 循环读取10个柱面
; 一张软盘有80个柱面，每个柱面有18个扇区，一个扇区512字节
; 读取下一个扇区要CL（扇区号）+1，ES（读入地址）+0x20（512/16）
; 柱面0，磁头0，扇区1读到柱面0，磁头0，扇区18后，
; 下一个是（磁盘反面）柱面0，磁头1，扇区1读到柱面0，磁头1，扇区18，
; 下一个是（磁盘正面）柱面1，磁头0，扇区1
; 然后一直读到柱面9，磁头1，扇区18
        MOV AX,ES
        ADD AX,0x0020; 内存地址后移0x20
        MOV ES,AX ; 因为没有ADD ES,0x20指令，所以这里绕个弯 
        ADD CL,1
        CMP CL,18
        JBE readloop ; 如果CL<=18跳转
        MOV CL,1 ;扇区
        ADD DH,1 ;磁头
        CMP DH,2
        JB readloop;DH<2跳转
        MOV DH,0
        ADD CH,1
        CMP CH,CYLS;CH<CYLS
        JB readloop
;运行haribote.sys
        MOV [0x0ff0],CH;记下IPL读到了什么程度
        JMP 0xc200
error:
        MOV SI,msg
putloop:
        MOV AL,[SI]
        ADD SI,1 ;SI加1
        CMP AL,0
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
        DB "load error"
        DB 0x0a
        DB 0
        ;RESB 0x7dfe-$
        RESB 0x1fe-($-$$);用0x00填满0x7dfe的命令	
        DB 0x55,0xaa
