#ifndef _SHEET_H_
#define _SHEET_H_
#include "memory.h"

#define MAX_SHEETS 256
#define SHEET_USE 1
#define SHEET_UNUSE 0

struct SHEET
{
    char *buf;
    int bxsize, bysize; /* sheet的长宽      */
    int vx0, vy0;       /* sheet的左上角坐标 */
    int col_inv;        /* 透明色           */
    int height;         /* sheet所在的高度   */
    int flags;          /* sheet的相关配置   */
    struct SHTCTL *shtctl;
};

struct SHTCTL
{
    unsigned char *vram, *map;  /* 显存初始地址和像素点所属sheet地图 */
    int xsize, ysize;           /* 显存长宽                       */
    int top;                    /* 当前最高sheet的height值        */
    struct SHEET *sheets[MAX_SHEETS];
    struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *shtctl);
void sheet_setbuf(struct SHEET *sheet, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sheet, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sheet, int x0, int y0);
void sheet_free(struct SHEET *sheet);

#endif
