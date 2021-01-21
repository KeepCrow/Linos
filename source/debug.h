#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "bootpack.h"

static unsigned int cur = 0;

#define debug_printf(args...) ({                                    \
    do                                                              \
    {                                                               \
        char msg[32];                                               \
        struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;   \
        int max_line = biinfo->scrny / LINE_SPAN;                   \
        sprintf(msg, args);                                         \
        line_show8(binfo->vram, binfo->scrnx, cur, msg);            \
        cur = (++cur >= max_line) ? (max_line - 1) : cur;           \
    }while(0);})

#define debug_print(msg) ({                                         \
    do                                                              \
    {                                                               \
        struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;   \
        int max_line = biinfo->scrny / LINE_SPAN;                   \
        line_show8(binfo->vram, binfo->scrnx, cur, msg);            \
        cur = (++cur >= max_line) ? (max_line - 1) : cur;           \
    } while (0);})

#endif
