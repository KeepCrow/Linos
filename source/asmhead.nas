; linos-os
; TAB=4

BOTPAK  EQU     0x00280000      ; 用于存放bootpack的目标地址
DSKCAC  EQU     0x00100000      ; 1MB地址处
DSKCAC0 EQU     0x00008000      ; 磁盘数据原本存放的地址

; 有关BOOT的信息
CYLS    EQU     0xff0       ; 存储启动区的地址
LEDS    EQU     0xff1       ; 存储Led灯数据的地址
VMODE   EQU     0xff2       ; 存储显示模式的地址
SCRNX   EQU     0xff4       ; 存储分辨率的X值的地址
SCRNY   EQU     0xff6       ; 存储分辨率的Y值的地址
VRAM    EQU     0xff8       ; 存储图像缓冲区地址的地址

        ORG     0xc200      ; 这部分程序存储的地址, 0xc200 = 0x8000 + 0x4200
                            ; 选择 0x8000 的原因是这一段内存是空闲的
                            ; 而 0x4200 是因为这部分的初始地址在整个img的0x4200位置

        MOV     AL, 0x13    ; VGA显卡, 320x200x8位彩色
        MOV     AH, 0x00
        ; MOV     BX, 0x4105  ; VBE的 640x480x8bit彩色
        ;                     ; 101------640x480   102------800x600
        ;                     ; 105------1024x768  107------1280x1024
        ; MOV     AX, 0x4f02
        INT     0x10

; 开始记录信息
        MOV     BYTE [VMODE], 8
        MOV     WORD [SCRNX], 320
        MOV     WORD [SCRNY], 200
        MOV     DWORD [VRAM], 0x000a0000    ; VRAM是0xa0000 - 0xaffff的64KB
        ; MOV     WORD [SCRNX], 1024
        ; MOV     WORD [SCRNY], 768
        ; MOV     DWORD [VRAM], 0xe0000000

; 利用BIOS取得键盘上各种LED指示灯的状态
        MOV     AH, 0x02
        INT     0x16
        MOV     [LEDS], AL


; PIC 关闭所有中断
; 根据AT兼容机的规格，如果要初始化PIC，必须在CLI前进行，否则有时会挂起
; 随后进行PIC的初始化
        MOV     AL, 0xff
        OUT     0x21, AL
        NOP                 ; 如果连续执行OUT指令，有些机器可能无法正常运行
        OUT     0xa1, AL

        CLI                 ; 禁止CPU级别的中断

; 为了让CPU访问1MB以上的内存，使能A20GATE
        CALL    waitkbdout
        MOV     AL,0xd1
        OUT     0x64,AL
        CALL    waitkbdout
        MOV     AL,0xdf  ; enable A20
        OUT     0x60,AL
        CALL    waitkbdout

; 切换到保护模式, 禁用分页的保护模式是分段保护模式
[INSTRSET "i486p"]  ; 开始使用i486指令的声明

        LGDT    [GDTR0] ; 临时GDT
        MOV     EAX,CR0
        AND     EAX,0x7fffffff   ; bit31为0 禁止分页
        OR      EAX,0x00000001    ; bit0为1 切换到保护模式
        MOV     CR0,EAX
        JMP     pipelineflush

pipelineflush:
        MOV     AX, 1*8  ;  可读写的段 32bit
        MOV     DS, AX
        MOV     ES, AX
        MOV     FS, AX
        MOV     GS, AX
        MOV     SS, AX

; bootpack的复制
        MOV     ESI, bootpack    ; 源地址
        MOV     EDI, BOTPAK  ; 目标地址
        MOV     ECX, 512*1024/4  ; 大小
        CALL    memcpy


; 磁盘数据的复制 送到它原本的位置

; 从启动扇区开始，将启动扇区从0x7c00复制到1MB地址
        MOV     ESI, 0x7c00  ; 源地址
        MOV     EDI, DSKCAC  ; 目标地址
        MOV     ECX, 512/4   ; 大小
        CALL    memcpy

; 磁盘剩余内容
        MOV     ESI, DSKCAC0+512 ; 源地址
        MOV     EDI, DSKCAC+512  ; 目标地址
        MOV     ECX, 0
        MOV     CL, BYTE [CYLS]
        IMUL    ECX, 512*18*2/4 ; 从柱面数变换为字节数/4
        SUB     ECX, 512/4   ; 减去IPL部分的大小
        CALL    memcpy

; 必须由asmhead完成的工作，至此全部完成

; 启动bootpack
        MOV     EBX, BOTPAK
        MOV     ECX, [EBX+16]
        ADD     ECX, 3   ; ECX += 3;
        SHR     ECX, 2   ; ECX /= 4;
        JZ      skip  ; Jump if zero 如果没有东西可以复制则跳转
        MOV     ESI, [EBX+20]    ; 源地址
        ADD     ESI, EBX
        MOV     EDI, [EBX+12]    ; 目标地址
        CALL    memcpy
skip:
        MOV     ESP,[EBX+12] ; ESP为栈的初始值
        JMP     DWORD 2*8:0x0000001b ; 跳到第二个段的0x1b地址，第二个段存储着bootpack.hrb

waitkbdout:
        IN      AL, 0x64
        AND     AL, 0x02
        IN      AL, 0x60  ; 空读
        JNZ     waitkbdout   ; AND的结果如果不是0, 就跳到waitkbdout
        RET

memcpy:
        MOV     EAX,[ESI]
        ADD     ESI,4
        MOV     [EDI],EAX
        ADD     EDI,4
        SUB     ECX,1
        JNZ     memcpy   ; 减法运算的结果不是0就继续移动
        RET
; memcpy�̓A�h���X�T�C�Y�v���t�B�N�X�����Y��Ȃ���΁A�X�g�����O���߂ł�������

        ALIGNB  16    ; 一直 DB 0 直到地址能被16整除
    ; 如果GDT0的地址不是8的倍数，处理起来会变慢
GDT0:
        RESB    8   ; NULL selector
        DW      0xffff,0x0000,0x9200,0x00cf   ; 可以读写的段 (segment) 32bit
        DW      0xffff,0x0000,0x9a28,0x0047   ; 可以执行的段 (segment) 32bit (bootpack用)

        DW      0
GDTR0:
        DW      8*3-1
        DD      GDT0

        ALIGNB  16
bootpack:
