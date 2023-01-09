[INSTRSET "i486p"]
[BITS 32]
		MOV		EDX,2
		MOV		EBX,msg
		INT		0x40
		MOV EDX,4 ; 这里！
		INT 0x40 ; 这里！

msg:
		DB	"hello",0
