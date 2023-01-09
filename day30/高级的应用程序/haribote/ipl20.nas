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
        MOV BX,18*2*CYLS-1 ; 要读取的合计扇区数
        CALL readfast ; 告诉读取
        ; 读取结束，运行haribote.sys
        MOV BYTE [0x0ff0],CYLS ; 记录IPL实际读取了多少内容
        JMP	0xc200
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

readfast: ; 使用AL尽量一次性读取数据 
        ; ES:读取地址, CH:柱面, DH:磁头, CL:扇区, BX:读取扇区数
        MOV AX,ES ; < 通过ES计算AL的最大值 >
        SHL AX,3 ; 将AX除以32，将结果存入AH（SHL是左移位指令）
        AND AH,0x7f ; AH是AH除以128所得的余数（512*128=64K）
        MOV AL,128 ; AL = 128 - AH; AH是AH除以128所得的余数（512*128=64K）
        SUB AL,AH
        MOV AH,BL ; < 通过BX计算AL的最大值并存入AH >
        CMP BH,0 ; if (BH != 0) { AH = 18; }
        JE .skip1
        MOV AH,18
.skip1:
        CMP AL,AH ; if (AL > AH) { AL = AH; }
        JBE .skip2
        MOV AL,AH
.skip2:
        MOV AH,19 ; < 通过CL计算AL的最大值并存入AH >
        SUB AH,CL ; AH = 19 - CL;
        CMP AL,AH ; if (AL > AH) { AL = AH; }
        JBE .skip3
        MOV AL,AH
.skip3:
        PUSH BX
        MOV SI,0 ; 计算失败次数的寄存器
retry:
        MOV AH,0x02 ; AH=0x02 : 读取磁盘
        MOV BX,0
        MOV DL,0x00 ; A盘
        PUSH ES
        PUSH DX
        PUSH CX
        PUSH AX
        INT 0x13 ; 调用磁盘BIOS函数
        JNC next ; 没有出错的话则跳转至next
        ADD SI,1 ; 将SI加1
        CMP SI,5 ; 将SI与5比较
        JAE error ; SI >= 5则跳转至error
        MOV AH,0x00
        MOV DL,0x00 ; A盘
        INT 0x13 ; 重置驱动器
        POP AX
        POP CX
        POP DX
        POP ES
        JMP retry

next:
; 循环读取20个柱面
; 一张软盘有80个柱面，每个柱面有18个扇区，一个扇区512字节
; 读取下一个扇区要CL（扇区号）+1，ES（读入地址）+0x20（512/16）
; 柱面0，磁头0，扇区1读到柱面0，磁头0，扇区18后，
; 下一个是（磁盘反面）柱面0，磁头1，扇区1读到柱面0，磁头1，扇区18，
; 下一个是（磁盘正面）柱面1，磁头0，扇区1
; 然后一直读到柱面9，磁头1，扇区18
        POP AX
        POP CX
        POP DX
        POP BX ; 将ES的内容存入BX
        SHR BX,5 ; 将BX由16字节为单位转换为512字节为单位
        MOV AH,0
        ADD BX,AX ; BX += AL;
        SHL BX,5 ; 将BX由512字节为单位转换为16字节为单位
        MOV ES,BX ; 相当于EX += AL * 0x20;
        POP BX
        SUB BX,AX
        JZ .ret
        ADD CL,AL ; 将CL加上AL
        CMP CL,18 ; 将CL与18比较
        JBE readfast ; CL <= 18则跳转至readfast
        MOV CL,1
        ADD DH,1
        CMP DH,2
        JB readfast ; DH < 2则跳转至readfast
        MOV DH,0
        ADD CH,1
        JMP readfast
.ret:
        RET 
        RESB 0x1fe-($-$$);用0x00填满0x7dfe的命令	
        DB 0x55,0xaa

