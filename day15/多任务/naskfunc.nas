; naskfunc 提供给c语言调用的基础函数
; TAB=4

[FORMAT "WCOFF"]        ; 目标文件格式
[INSTRSET "i486p"]      ; 告诉编译器这个程序给486用的
[BITS 32]               ; 制作成32位模式用的的机器语言
[FILE "naskfunc.asm"]   ; 源文件名信息

        ; 程序中包含的函数名
		;GLOBAL _write_mem8 ;此函数功能是向指定地址写入值，c语言的指针可以代替它实现这个功能，所以注释掉
        GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_taskswitch3, _taskswitch4,_farjmp
		GLOBAL	_asm_inthandler20, _asm_inthandler21
		GLOBAL	_asm_inthandler27, _asm_inthandler2c
		GLOBAL	_memtest_sub
		EXTERN	_inthandler20, _inthandler21
		EXTERN	_inthandler27, _inthandler2c

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

; 将段上限和地址赋值给GDTR寄存器（48位）
; 不能用mov赋值
; 该寄存器低16位是段上限=GDT有效字节数-1，高32位标识GDT的开始地址
_load_gdtr:     ; void load_gdtr(int limit, int addr);
        MOV     AX,[ESP+4]  ; limit
        MOV     [ESP+6],AX
        LGDT    [ESP+6]
        RET

_load_idtr:     ; void load_idtr(int limit, int addr);
        MOV     AX,[ESP+4]  ; limit
        MOV     [ESP+6],AX
        LIDT    [ESP+6]
        RET

; 32位模式下，汇编语言和c语言连用的话，能够读写的寄存器只有EAX、ECX、EDX，其它只能读
; _write_mem8: ;void write_mem8(int addr,int data)
;         MOV ECX,[ESP+4] ;[ESP+4]中存放的是地址，将其读入ecx
;         MOV AL,[ESP+8]  ;[ESP+8]中存放的是地址，将其读入al
;         MOV [ECX],AL
;         RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; 乮EBX, ESI, EDI 傕巊偄偨偄偺偱乯
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_load_tr:		; void load_tr(int tr);
		LTR		[ESP+4]					; [ESP+4]表示参数tr		
		RET

_taskswitch3:	; void taskswitch4(void); 切换任务3
		JMP		3*8:0
		RET

_taskswitch4:	; void taskswitch4(void); 切换任务4
		JMP		4*8:0
		RET


_farjmp: ; void farjmp(int eip, int cs);
JMP FAR [ESP+4] ; eip, cs “JMP FAR”指令的功能是执行far跳转
RET