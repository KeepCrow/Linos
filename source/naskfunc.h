#ifndef _NASKFUNC_H_
#define _NASKFUNC_H_

void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_out8(int add, int data);
void io_out16(int add, int data);
void io_out32(int add, int data);
int io_in8(int add);
int io_in16(int add);
int io_in32(int add);
int io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

#endif
