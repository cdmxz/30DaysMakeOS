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

		; 防止PIC接受所有中断
		; 在AT兼容机的规格中，如果要初始化PIC
		; 如果不在CLI前做这个的话，偶尔会死机
		; PIC的初始化以后再做

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 不断执行OUT指令
		OUT		0xa1,AL

		CLI						; 此外，CPU级别也禁止中断

; 让CPU支持1M以上内存、设置A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 保护模式转换

[INSTRSET "i486p"]				; 说明使用486指令

		LGDT	[GDTR0]			; 设置临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设置bit31为0（禁止分页）
		OR		EAX,0x00000001	; 将bit0设为1（保护模式过渡）
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  読み書き可能セグメント32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpackの転送

		MOV		ESI,bootpack	; 源
		MOV		EDI,BOTPAK		; 目标
		MOV		ECX,512*1024/4
		CALL	memcpy

; ついでにディスクデータも本来の位置へ転送

; まずはブートセクタから

		MOV		ESI,0x7c00		; 転送元
		MOV		EDI,DSKCAC		; 転送先
		MOV		ECX,512/4
		CALL	memcpy

; 残り全部

		MOV		ESI,DSKCAC0+512	; 転送元
		MOV		EDI,DSKCAC+512	; 転送先
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; シリンダ数からバイト数/4に変換
		SUB		ECX,512/4		; IPLの分だけ差し引く
		CALL	memcpy

; asmheadでしなければいけないことは全部し終わったので、
;	あとはbootpackに任せる

; bootpackの起動

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 転送するべきものがない
		MOV		ESI,[EBX+20]	; 転送元
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 転送先
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; スタック初期値
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; ANDの結果が0でなければwaitkbdoutへ
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 引き算した結果が0でなければmemcpyへ
		RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGNB	16
GDT0:
		RESB	8				; ヌルセレクタ
		DW		0xffff,0x0000,0x9200,0x00cf	; 読み書き可能セグメント32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 実行可能セグメント32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
