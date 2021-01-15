#include "sheet.h"
#include "debug.h"

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
    int i;
    struct SHTCTL *shtctl;
    shtctl = (struct SHTCTL *)memman_alloc4k(memman, sizeof(struct SHTCTL));
    if (shtctl == 0)
        return 0;
    
    shtctl->vram  = vram;
    shtctl->xsize = xsize;
    shtctl->ysize = ysize;
    shtctl->top   = -1;
    for (i = 0; i < MAX_SHEETS; i++)
        shtctl->sheets0[i].flags = SHEET_UNUSE;
    
    return shtctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *shtctl)
{
    int i;
    struct SHEET *sht;
    for (i = 0; i < MAX_SHEETS; i++)
    {
        if (shtctl->sheets0[i].flags == SHEET_UNUSE)
        {
            sht = &(shtctl->sheets0[i]);
            sht->height = -1;   /* 标记为不显示 */
            sht->flags  = SHEET_USE;
            return sht; 
        }
    }
    return 0;   /* 所有的sheet都处于使用状态, 当然这种情况感觉不太可能发生的样子 */
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
    sht->buf     = buf;
    sht->bxsize  = xsize;
    sht->bysize  = ysize;
    sht->col_inv = col_inv;
    return;
}

/**
 * @brief 将[st, ed]范围的sheet全部上移一层
 *        NOTE: sheets[ed + 1]的元素会被覆盖
 * 
 * @param shtctl sheet管理器
 * @param st     上移的最低层的高度
 * @param ed     上移的最高层的高度
 */
static inline void sheet_up(struct SHTCTL *shtctl, int st, int ed)
{
    int i;
    
    if (ed == shtctl->top)
        shtctl->top += 1;

    for (i = ed; i >= st; i--)
    {
        shtctl->sheets[i]->height += 1;
        shtctl->sheets[i + 1] = shtctl->sheets[i];
    }

}

/**
 * @brief 将[st, ed]范围的sheet全部下移一层
 *        NOTE: sheets[st - 1]的元素会被覆盖
 * 
 * @param shtctl sheet管理器
 * @param st     下移的最低层的高度
 * @param ed     下移的最高层的高度
 */
static inline void sheet_down(struct SHTCTL *shtctl, int st, int ed)
{
    int i;

    if (ed == shtctl->top)
        shtctl->top -= 1;

    for (i = st; i <= ed; i++)
    {
        shtctl->sheets[i]->height -= 1;
        shtctl->sheets[i - 1] = shtctl->sheets[i];
    }
}

void sheet_updown(struct SHTCTL *shtctl, struct SHEET *sht, int height)
{
    int h;
    int old = sht->height;

    /* 如果height过高或过低, 修正之 */
    height = (height > shtctl->top) ? (shtctl->top + 1) : height;
    height = (height < -1) ? -1 : height;
    sht->height = height;
    
    if (old == height)
        return;

    if (old == -1)          /* 显示sheet */
    {
        sheet_up(shtctl, height, shtctl->top);
        shtctl->sheets[height] = sht;
    }
    else if (height == -1)  /* 隐藏sheet */
    {
        sheet_down(shtctl, height + 1, shtctl->top);
    }
    else if (old < height)  /* 升高sheet */
    {
        sheet_down(shtctl, old + 1, height);
        shtctl->sheets[height] = sht;
    }
    else if (old > height)  /* 降低sheet */
    {
        sheet_up(shtctl, height, old - 1);
        shtctl->sheets[height] = sht;
    }

    sheet_refresh(shtctl, sht, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
    return;
}

void sheet_refresh(struct SHTCTL *shtctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
    if (sht->height >= 0)
        sheet_refreshsub(shtctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
    return;
}

void sheet_refreshsub(struct SHTCTL *shtctl, int vx0, int vy0, int vx1, int vy1)
{
    int h, bx, by, vx, vy;
    int bx0, by0, bx1, by1; /* sheet内相对坐标 */
    unsigned char c;
    struct SHEET *sht;

    for (h = 0; h <= shtctl->top; h++)
    {
        sht = shtctl->sheets[h];

        if (vx0 < 0) vx0 = 0;
        if (vy0 < 0) vy0 = 0;
        if (vx1 > shtctl->bxsize) vx1 = shtctl->xsize;
        if (vy1 > shtctl->ysize) vy1 = shtctl->ysize;

        /* 通过绝对坐标计算相对坐标 */
        bx0 = vx0 - sht->vx0;
        by0 = vy0 - sht->vy0;
        bx1 = vx1 - sht->vx0;
        by1 = vy1 - sht->vy0;

        if (bx0 < 0) bx0 = 0;
        if (by0 < 0) by0 = 0;
        if (bx1 > sht->bxsize) bx1 = sht->bxsize;
        if (by1 > sht->bysize) by1 = sht->bysize;

        for (by = by0; by < by1; by++)
        {
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++)
            {
                vx = sht->vx0 + bx;
                c  = sht->buf[by * sht->bxsize + bx];
                if (c != sht->col_inv) 
                    shtctl->vram[vy * shtctl->xsize + vx] = c;
            }
        }
    }
    return;
}

void sheet_slide(struct SHTCTL *shtctl, struct SHEET *sht, int vx0, int vy0)
{
    int oldx = sht->vx0;
    int oldy = sht->vy0;
    sht->vx0 = vx0;
    sht->vy0 = vy0;
    if (sht->height >= 0)
    {
        sheet_refreshsub(shtctl, oldx, oldy, oldx + sht->bxsize, oldy + sht->bysize);
        sheet_refreshsub(shtctl, vx0,  vy0,  vx0 + sht->bxsize,  vy0 + sht->bysize);
    }
    return;
}

void sheet_free(struct SHTCTL *shtctl, struct SHEET *sht)
{
    if (sht->height >= 0)
        sheet_updown(shtctl, sht, -1);
    sht->flags  = 0;
    return;
}
