#include "task.h"

struct TSS32 tss_a, tss_b;

void tss_init(struct TSS32 *tss)
{
    tss->ldtr  = 0;
    tss->iomap = 0x40000000;
}
