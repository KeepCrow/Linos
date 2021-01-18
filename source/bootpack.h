#ifndef _BOOTPACK_H_
#define _BOOTPACK_H_

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
