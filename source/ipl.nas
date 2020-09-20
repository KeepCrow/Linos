; lin-os
; TAB = 4

    ORG 0x7c00      ; 指明程序的装在地址

; 以下这段是标准FAT12格式软盘专用代码
    JMP entry
    DB 0x90
    DB "LINOS   "   ; 启动区的名称，任意8字节的字符串
    DW 512          ; 每个扇区的大小，必须是512字节
    DB 1            ; 簇的大小，必须为一个扇区
    DW 1            ; FAT的起始位置，一般从第一个扇区开始
    DB 2            ; FAT的个数，必须为2？个数什么意思
    DW 224          ; 根目录的大小，一般设为224？根目录大小什么意思
    DW 2880         ; 该磁盘的大小，必须是2880扇区
    DB 0xf0         ; 磁盘的种类，必须是0xf0
    DW 9            ; FAT的长度，必须是9扇区；长度什么意思
    DW 18           ; 1个磁道
    DW 2            ; 磁头数，必须是2
    DD 0            ; 不使用分区，必须为0
    DD 2880         ; 重写一次的磁盘大小
    DB 0, 0, 0x29   ; 意义不明，固定
    DD 0xffffffff   ; 可能是卷标号码
    DB "LINOS      "; 磁盘名称，11字节的字符串
    DB "FAT12   "   ; 磁盘格式名称，8字节字符串
    RESB 18         ; 先空出18字节

entry:
    MOV AX, 0       ; 初始化寄存器
    MOV SS, AX      ; SS为栈段寄存器，段选择符
    MOV SP, 0x7c00
    MOV DS, AX      ; DS为数据段寄存器，段选择符

; 读取磁盘中的数据
    MOV AX, 0x820
    MOV ES, AX      ; ES:BX表示目标内存地址
    MOV BX, 0       ; 地址为：0x8200
    MOV CH, 0       ; 柱面0
    MOV DH, 0       ; 磁头0
    MOV CL, 2       ; 扇区2

    MOV AH, 0x02    ; 0x02表示读盘
    MOV AL, 1       ; 同时处理的扇区数为1
    MOV DL, 0x00    ; 驱动器号0
    INT 0x12        ; 12号中断
    JC error

fin:
    HLT             ; 让CPU无限循环，等待指令
    JMP fin         ; 无限循环

error:
    MOV SI, msg

putloop:
    ; 显示一个字符
    MOV AL, [SI]    
    CMP AL, 0       ; 如果字符为0，则停止显示
    JE fin
    MOV AH, 0x0e    ; 显示一个字符
    MOV BX, 15      ; 
    INT 0x10        ; 调用显卡BIOS

    ADD SI, 1       ; 处理下一个字节
    JMP putloop

msg:
    DB 0x0a, 0x0a   ; 换行2次
    DB "load error"
    DB 0x0a
    DB 0

    ; 0x7c00 + 0x200(512) - 0x02(2) = 0x7c00 + 0x1fe = 0x7dfe
    RESB 0x7dfe-$
    DB 0x55, 0xaa