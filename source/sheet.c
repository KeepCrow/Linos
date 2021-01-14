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
 * 
 * @param shtctl sheet管理器
 * @param st     上移的最低层的高度
 * @param ed     上移的最高层的高度
 */
static void sheet_rise(struct SHTCTL *shtctl, int st, int ed)
{
    int i;
    for (i = ed; i > st; i++)
    {
        shtctl->sheets[i]->height += 1;
        shtctl->sheets[i] = shtctl->sheets[i - 1];
    }
}

void sheet_updown(struct SHTCTL *shtctl, struct SHEET *sht, int height)
{
    int h;
    int old = sht->height;

    /* 如果height过高或过低, 修正 */
    height = (height > shtctl->top) ? (shtctl->top + 1) : height;
    height = (height < -1) ? -1 : height;
    sht->height = height;
    
    if (old == height)
    {
        return;
    }

    /* 显示sheet */
    if (old == -1)
    {
        for (h = shtctl->top; h > height; h--)
        {
            shtctl->sheets[h]->height += 1;
            shtctl->sheets[h] = shtctl->sheets[h - 1];
        }
        shtctl->top += 1;
        shtctl->sheets[height] = sht;
    }

    /* 隐藏sheet */
    else if (height == -1)
    {
        for (h = old; h < shtctl->top; h++)
        {
            shtctl->sheets[h]->height -= 1;
            shtctl->sheets[h] = shtctl->sheets[h + 1];
        }
        shtctl->top -= 1;
    }

    /* 升高sheet */
    else if (old < height)
    {
        for (h = old; h < height; h++)
        {
            shtctl->sheets[h]->height -= 1;
            shtctl->sheets[h] = shtctl->sheets[h + 1];
        }
        shtctl->sheets[height] = sht;
    }

    /* 降低sheet */
    else if (old > height)
    {
        for (h = old; h > height; h++)
        {
            shtctl->sheets[h]->height += 1;
            shtctl->sheets[h] = shtctl->sheets[h - 1];
        }
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
    unsigned char c;
    struct SHEET *sht;

    for (h = 0; h <= shtctl->top; h++)
    {
        sht = shtctl->sheets[h];
        for (by = 0; by < sht->bysize; by++)
        {
            vy = sht->vy0 + by;
            for (bx = 0; bx < sht->bxsize; bx++)
            {
                vx = sht->vx0 + bx;
                if (vx0 <= vx && vx < vx1 && vy0 <= vy && vy < vy1) 
                {
                    c  = sht->buf[by * sht->bxsize + bx];
                    if (c != sht->col_inv) 
                        shtctl->vram[vy * shtctl->xsize + vx] = c;
                }
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
