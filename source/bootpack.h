#ifndef _BOOTPACK_H_
#define _BOOTPACK_H_

#include "dsctbl.h"
#include "fifo.h"
#include "graphic.h"
#include "int.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "naskfunc.h"
#include "sheet.h"
#include "window.h"

#define ADR_BOOTINFO    0x00000ff0

struct BOOTINFO
{
    char cyls;
    char leds;
    char vmode;
    char reserve;
    short scrnx, scrny;
    char *vram;
};

#endif
