; haribote-ipl ���س���
; TAB=4

CYLS    EQU 20; �����#define cyls 20  Ҫ�����������
        ORG 0x7c00  ; ָ����������װ�ص�ַ ����Ϊ0x7c00-0x7dff֮��


; ���´�����������FAT12��ʽ����

        JMP entry
        DB  0x90
        DB  "HARIBOTE" ; ������������������ַ���(8�ֽ�)
        DW  512 ; ÿ�������Ĵ�С �̶�512�ֽ�
        DB  1 ; Ӳ�̴ش�С �̶�1������
        DW  1 ; FAT ����ʼλ�� һ��ӵ�һ������ʼ
        DB  2 ; FAT�ĸ�������Ϊ2
        DW  224 ; ��Ŀ¼�Ĵ�С һ��Ϊ225��
        DW  2880 ; ���̴�С ����2880����
        DB  0xf0 ; �������� �̶�0xf0
        DW  9 ; FAT����
        DW  18 ; 1���ŵ��м�������18
        DW  2 ; ��ͷ��������2
        DD  0 ; ��ʹ�÷���������0
        DD  2880 ; ��дһ�δ��̴�С
        DB  0,0,0x29 ; ���岻�����̶�
        DD  0xffffffff ; ������
        DB  "HARIBOTEOS " ;�������� 11�ֽ�
        DB  "FAT12   " ; ���̸�ʽ���� 8�ֽ�
        RESB 18 ; �ȿճ�18�ֽ�        

; ��������

entry:
        MOV AX,0    ; ��ʼ���Ĵ���
        MOV SS,AX
        MOV SP,0x7c00
        MOV DS,AX

; ������

        MOV AX,0x0820 ; ��������װ�ص�ַ������0x7dff��0x7c00-0x7dff���������ˣ�֮��
        MOV ES,AX
        ; �趨�Ĵ�����ֵ=����
        MOV CH,0 ; ����0 ��������Χ��һȦ
        MOV DH,0 ; ��ͷ0 ���������ͷ
        MOV CL,2 ; ����2 һ������512�ֽ�
readloop:
        MOV SI,0 ; ���ؼ�¼ʧ�ܴ��� �������̳�磬ż������������

retry:
        MOV AH,0x02 ; AH=0x02 �������
        MOV AL,1; 1������
        MOV BX,0
        MOV DL,0x00;A������
        INT 0x13;����BIOS���̺���
        JNC next;��λ��־Ϊ0��ת���ɹ���next 
        ADD SI,1;SI+1
        CMP SI,5; �Ƚ�SI��5
        JAE error; SI >= 5����תerror
        MOV AH,0x00
        MOV DL,0x00
        INT 0x13; ����������
        JMP retry
next:
; ѭ����ȡ10������
; һ��������80�����棬ÿ��������18��������һ������512�ֽ�
; ��ȡ��һ������ҪCL�������ţ�+1��ES�������ַ��+0x20��512/16��
; ����0����ͷ0������1��������0����ͷ0������18��
; ��һ���ǣ����̷��棩����0����ͷ1������1��������0����ͷ1������18��
; ��һ���ǣ��������棩����1����ͷ0������1
; Ȼ��һֱ��������9����ͷ1������18
        MOV AX,ES
        ADD AX,0x0020; �ڴ��ַ����0x20
        MOV ES,AX ; ��Ϊû��ADD ES,0x20ָ����������Ƹ��� 
        ADD CL,1
        CMP CL,18
        JBE readloop ; ���CL<=18��ת
        MOV CL,1 ;����
        ADD DH,1 ;��ͷ
        CMP DH,2
        JB readloop;DH<2��ת
        MOV DH,0
        ADD CH,1
        CMP CH,CYLS;CH<CYLS
        JB readloop
;����haribote.sys
        MOV [0x0ff0],CH;����IPL������ʲô�̶�
        JMP 0xc200
error:
        MOV SI,msg
putloop:
        MOV AL,[SI]
        ADD SI,1 ;SI��1
        CMP AL,0
        JE fin
        MOV AH,0x0e ; ��ʾһ������
        MOV BX,15 ; �ַ���ɫ
        INT 0x10; �����Կ�bios
        JMP putloop
fin:
        HLT ;��cpuֹͣ���ȴ�ָ��
        JMP fin; ����ѭ��

msg:
        DB 0x0a,0x0a;����2��
        DB "load error"
        DB 0x0a
        DB 0
        ;RESB 0x7dfe-$
        RESB 0x1fe-($-$$);��0x00����0x7dfe������	
        DB 0x55,0xaa
