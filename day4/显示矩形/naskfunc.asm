; naskfunc 提供给c语言调用的基础函数
; TAB=4

[FORMAT "WCOFF"] ; 目标文件格式
[INSTRSET "i486p"]; 告诉编译器这个程序给486用的
[BITS 32]         ; 制作成32位模式用的的机器语言
[FILE "naskfunc.asm"]; 源文件名信息

        ; 程序中包含的函数名
        GLOBAL _io_hlt
        ;GLOBAL _write_mem8 ;此函数功能是向指定地址写入值，c语言的指针可以代替它实现这个功能，所以注释掉
        GLOBAL _io_cli
        GLOBAL _io_sti
        GLOBAL _io_stihlt
        GLOBAL _io_in8
        GLOBAL _io_in16
        GLOBAL _io_in32
        GLOBAL _io_out8
        GLOBAL _io_out16
        GLOBAL _io_out32
        GLOBAL _io_load_eflags
        GLOBAL _io_store_eflags

; 以下是实际的函数
[SECTION .text]; 要先写这些在再写程序实现

_io_hlt:     ;void io_hle(void);
        HLT
        RET ; 返回

_io_cli:    ; void io_cli(void);
        CLI ; 将中断标志置为0 clear interrupt flag 
            ; 当cpu遇到中断请求时，这个标志告诉cpu是否处理中断请求
        RET
        
_io_sti:    ; void io_sti(void);
        STI ; 将中断标志置为1 set interrupt flag
        RET

_io_stihlt:    ; void io_stihlt(void);
        STI
        HLT
        RET

_io_in8:    ; int io_in8(void);
        MOV     EDX,[ESP+4] ;port
        MOV     EAX,0
        IN      AL,DX
        RET

_io_in16:   ; int io_in16(void);
        MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:   ; int io_in32(void);
        MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:   ; void io_out8(void);
        MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:  ; void io_out16(void);
        MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:  ; void io_out32(void);
        MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

; 读取eflags寄存器
_io_load_eflags:    ; int io_load_eflags(void);
        PUSHFD		; PUSHFD(Push eflags double word) 将标志位的值压入栈
		POP		EAX
		RET ; 返回EAX的值

; 写eflags寄存器 
_io_store_eflags:   ; void io_store_eflags(void);
        MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POPFD(pop flags double flag) 将标志位从栈弹出
		RET



; 32位模式下，和c语言连用的话，能够读写的寄存器只有EAX、ECX、EDX，其它只能读
; _write_mem8: ;void write_mem8(int addr,int data)
;         MOV ECX,[ESP+4] ;[ESP+4]中存放的是地址，将其读入ecx
;         MOV AL,[ESP+8]  ;[ESP+8]中存放的是地址，将其读入al
;         MOV [ECX],AL
;         RET
