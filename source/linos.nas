; linos-os
; TAB=4

    ORG     0xc200      ; 这部分程序存储的地址

    MOV     AL, 0x13    ; VGA显卡, 320x200x8位彩色
    MOV     AH, 0x00    
    INT     0x10

fin:
    HLT
    JMP fin