#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "bootpack.h"

static unsigned int cur = 0;

#define debug_printf(args...) ({                                    \
    do                                                              \
    {                                                               \
        char msg[32];                                               \
        struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;   \
        sprintf(msg, args);                                         \
        show_line8(binfo->vram, binfo->scrnx, cur, msg);            \
        cur = (++cur >= MAX_LINE) ? (MAX_LINE - 1) : cur;           \
    }while(0);})

#define debug_print(msg) ({                                         \
    do                                                              \
    {                                                               \
        struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;   \
        show_line8(binfo->vram, binfo->scrnx, cur, msg);            \
        cur = (++cur >= MAX_LINE) ? (MAX_LINE - 1) : cur;           \
    } while (0);})

#endif
