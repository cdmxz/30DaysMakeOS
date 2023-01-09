; haribote-os 启动程序
; TAB=4

BOTPAK	EQU		0x00280000	; 加载bootpack
DSKCAC	EQU		0x00100000	; 磁盘缓存的位置
DSKCAC0	EQU		0x00008000	; 磁盘缓存的位置（实模式）

; BOOT_INFO
CYLS	EQU		0x0ff0      ;设定引导扇区
LEDS	EQU 	0x0ff1 
VMODE	EQU 	0x0ff2      ;关于颜色数目的信息，颜色的位数
SCRNX	EQU 	0x0ff4      ;分辨率x
SCRNY	EQU 	0x0ff6      ;分辨率y
VRAM	EQU 	0x0ff8      ;图像缓冲区开始的地址

		ORG 	0xc200      ; 用16进制编辑器打开，文件名在0x002600
                            ; 文件内容在0x004200
                            ; 所以启动扇区加载完后跳转到0x7c00（软盘内容装载地址）+0x004200
; 画面设置

		MOV 	AL,0x13     ; VGA显卡，320x200x8位彩色
		MOV 	AH,0x00
		INT 	0x10        ;调用显卡bios函数
        ;将画面模式，分辨率保存到内存
		MOV 	BYTE [VMODE],8
		MOV 	WORD [SCRNX],320
		MOV 	WORD [SCRNY],200
		MOV 	DWORD [VRAM],0x000a0000

;用BIOS取得键盘上各种LED指示灯的状态
		MOV AH,0x02
		INT 0x16         ;键盘BIOS函数
		MOV [LEDS],AL

		; PIC关闭一切中断
		; 根据AT兼容机的规格，如果要初始化PIC，
		; 必须在CLI之前进行，否则偶尔会死机
		; 随后进行PIC的初始化。
        ; 等同于以下内容的C程序。
        ; io_out(PIC0_IMR, 0xff); /* 禁止主PIC的全部中断 */
        ; io_out(PIC1_IMR, 0xff); /* 禁止从PIC的全部中断 */
        ; io_cli(); /* 禁止CPU级别的中断*/
		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 如果连续执行OUT指令，有些机种会无法正常运行
		OUT		0xa1,AL

		CLI						; 禁止CPU级别的中断


; 为了让CPU能够访问1MB以上的内存空间，设定A20GAT

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; 让A20GATE信号线变成ON的状态
		OUT		0x60,AL
		CALL	waitkbdout

; 切换到保护模式
[INSTRSET "i486p"]				; 说明使用486指令 为了能够使用386以后的LGDT，EAX，CR0等关键字

		LGDT	[GDTR0]			; 设置临时GDT
        ; 将CR0寄存器的最高位置为0，最低位置为1进入保护模式
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设置bit31为0（禁止分页）
		OR		EAX,0x00000001	; 将bit0设为1（为了切换到保护模式）
		MOV		CR0,EAX
		JMP		pipelineflush   ; 切换到保护模式时，要马上执行JMP指令
pipelineflush:
		MOV		AX,1*8			; 可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的转送
; memcpy(bootpack, BOTPAK, 512*1024/4); 
; 从bootpack的地址开始的512KB内容复制到0x00280000号地址去
; haribote.sys是通过asmhead.bin和bootpack.hrb连接起来而生成的
; 所以asmhead结束的地方，紧接着串连着bootpack.hrb最前面的部分。

		MOV		ESI,bootpack	; 源
		MOV		EDI,BOTPAK		; 目标
		MOV		ECX,512*1024/4
		CALL	memcpy

; 磁盘数据最终转送到它本来的位置去

; 首先从启动扇区开始
		MOV		ESI,0x7c00		; 转送源
		MOV		EDI,DSKCAC		; 转送目标
		MOV		ECX,512/4
		CALL	memcpy
        ; 上面几句 = memcpy(0x7c00, DSKCAC, 512/4);
        ; 转送数据大小是以双字（4字节）为单位的，所以数据大小用字节数除以4来指定
        ; 表示是将启动扇区复制到1MB以后的内存
        ; memcpy(转送源地址, 转送目的地址, 转送数据的大小);
; 所有剩下的
        ; memcpy(DSKCAC0+512, DSKCAC+512, cyls * 512*18*2/4-512/4);
		MOV		ESI,DSKCAC0+512	; 转送源
		MOV		EDI,DSKCAC+512	; 转送目标
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 从柱面数变换为字节数/4
		SUB		ECX,512/4		; 减去 IPL
		CALL	memcpy

; 必须由asmhead来完成的工作，至此全部完毕
; 以后就交由bootpack来完成
; bootpack的启动
		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 没有要转送的东西时
		MOV		ESI,[EBX+20]	; 转送源
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 转送目的地
		CALL	memcpy          ; 将bootpack.hrb第0x10c8字节开始
                                ; 的0x11a8字节复制到0x00310000号地址去
skip:
		MOV		ESP,[EBX+12]	; 栈初始值
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
        IN       AL,0x60        ; 清空数据接收缓冲区中的垃圾数据
        ;如果控制器里有键盘代码，或者是鼠标数据 那就清空
		JNZ		waitkbdout		; AND的结果如果不是0，就跳到waitkbdout
		RET
        
; memcpy(转送源地址, 转送目的地址, 转送数据的大小);
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 减法运算的结果如果不是0，就跳转到memcpy
		RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGNB	16
GDT0:
		RESB	8				; NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	 ; 可以读写的段（segment）32bit
        DW      0xffff,0x0000,0x9a28,0x0047 ; 可以执行的段（segment）32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
