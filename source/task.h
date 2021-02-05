#ifndef _TASK_H_
#define _TASK_H_

struct TSS32
{
    /* 任务设置相关信息 */
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    /* 32位寄存器 */
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    /* 16位寄存器    */
    int es, cs, ss, ds, fs, gs;
    int ldtr, iomap;
};

void tss_init(struct TSS32 *tss);
void farjmp(int eip, int cs);

#endif
