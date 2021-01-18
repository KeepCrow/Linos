#include "sheet.h"
#include "debug.h"

struct SHTCTL *shtctl_init(struct MEMMAN *man, unsigned char *vram, int xsize, int ysize)
{
    int i;
    struct SHTCTL *shtctl;
    shtctl = (struct SHTCTL *)memman_alloc4k(man, sizeof(struct SHTCTL));
    if (shtctl == 0)
        return 0;    

    shtctl->map = (unsigned char *)memman_alloc4k(man, xsize * ysize);
    if (shtctl->map == 0)
    {
        memman_free4k(man, (unsigned int)shtctl, sizeof(struct SHTCTL));
        return 0;
    }

    shtctl->vram  = vram;
    shtctl->xsize = xsize;
    shtctl->ysize = ysize;
    shtctl->top   = -1;
    for (i = 0; i < MAX_SHEETS; i++)
    {
        shtctl->sheets0[i].flags = SHEET_UNUSE;
        shtctl->sheets0[i].shtctl   = shtctl;
    }
    
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

void sheet_refreshmap(struct SHTCTL *shtctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
    int vx, vy;
    int h, bx, by;
    int bx0, by0, bx1, by1;
    unsigned char sid;
    struct SHEET *sht;

    vx0 = (vx0 < 0) ? 0 : vx0;
    vy0 = (vy0 < 0) ? 0 : vy0;
    vx1 = (vx1 > shtctl->xsize) ? shtctl->xsize : vx1;
    vy1 = (vy1 > shtctl->ysize) ? shtctl->ysize : vy1;

    for (h = h0; h <= shtctl->top; h++)
    {
        sht = shtctl->sheets[h];
        sid = sht - shtctl->sheets0;

        bx0 = (vx0 - sht->vx0) < 0 ? 0 : (vx0 - sht->vx0);
        by0 = (vy0 - sht->vy0) < 0 ? 0 : (vy0 - sht->vy0);
        bx1 = (vx1 - sht->vx0) > sht->bxsize ? sht->bxsize : (vx1 - sht->vx0);
        by1 = (vy1 - sht->vy0) > sht->bysize ? sht->bysize : (vy1 - sht->vy0);

        for (by = by0; by < by1; by++)
        {
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++)
            {
                vx = sht->vx0 + bx;
                if (sht->buf[by * sht->bysize + bx] != sht->col_inv)
                    shtctl->map[vy * shtctl->xsize + vx] = sid;
            }
        }
    }
    return;
}

static void sheet_refreshsub(struct SHTCTL *shtctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
    int vx, vy;
    int h, bx, by;
    int bx0, by0, bx1, by1; /* sheet内相对坐标 */
    int xsize, ysize, bxsize, bysize;
    unsigned char sid;
    struct SHEET *sht;

    xsize = shtctl->xsize;
    ysize = shtctl->ysize;
    vx0 = (vx0 < 0) ? 0 : vx0;
    vy0 = (vy0 < 0) ? 0 : vy0;
    vx1 = (vx1 > xsize) ? xsize : vx1;
    vy1 = (vy1 > ysize) ? ysize : vy1;

    for (h = h0; h <= h1; h++)
    {
        sht = shtctl->sheets[h];
        sid = sht - shtctl->sheets0;
        bxsize = sht->bxsize;
        bysize = sht->bysize;

        /* 通过绝对坐标计算相对坐标 */
        bx0 = (vx0 - sht->vx0) < 0 ? 0 : (vx0 - sht->vx0);
        by0 = (vy0 - sht->vy0) < 0 ? 0 : (vy0 - sht->vy0);
        bx1 = (vx1 - sht->vx0) > bxsize ? bxsize : (vx1 - sht->vx0);
        by1 = (vy1 - sht->vy0) > bysize ? bysize : (vy1 - sht->vy0);

        for (by = by0; by < by1; by++)
        {
            vy = sht->vy0 + by;
            for (bx = bx0; bx < bx1; bx++)
            {
                vx = sht->vx0 + bx;
                if (shtctl->map[vy * xsize + vx] == sid) 
                    shtctl->vram[vy * xsize + vx] = sht->buf[by * bxsize + bx];
            }
        }
    }
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
    return;
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
    return;
}

void sheet_updown(struct SHEET *sht, int height)
{
    int old = sht->height;
    int vx0, vy0, vx1, vy1;
    struct SHTCTL *shtctl = sht->shtctl;

    /* 如果height过高或过低, 修正之 */
    height = (height > shtctl->top) ? (shtctl->top + 1) : height;
    height = (height < -1) ? -1 : height;
    sht->height = height;
    vx0 = sht->vx0;
    vy0 = sht->vy0;
    vx1 = vx0 + sht->bxsize;
    vy1 = vy0 + sht->bysize;

    if (old == height)
        return;

    if (old == -1)          /* 显示sheet */
    {
        sheet_up(shtctl, height, shtctl->top);
        shtctl->sheets[height] = sht;
        sheet_refreshmap(shtctl, vx0, vy0, vx1, vy1, height);
        sheet_refreshsub(shtctl, vx0, vy0, vx1, vy1, height, height);
    }
    else if (height == -1)  /* 隐藏sheet */
    {
        sheet_down(shtctl, height + 1, shtctl->top);
        sheet_refreshmap(shtctl, vx0, vy0, vx1, vy1, 0);
        sheet_refreshsub(shtctl, vx0, vy0, vx1, vy1, 0, old - 1);
    }
    else if (old < height)  /* 升高sheet */
    {
        sheet_down(shtctl, old + 1, height);
        shtctl->sheets[height] = sht;
        sheet_refreshmap(shtctl, vx0, vy0, vx1, vy1, height);
        sheet_refreshsub(shtctl, vx0, vy0, vx1, vy1, height, height);
    }
    else if (old > height)  /* 降低sheet */
    {
        sheet_up(shtctl, height, old - 1);
        shtctl->sheets[height] = sht;
        sheet_refreshmap(shtctl, vx0, vy0, vx1, vy1, height + 1);
        sheet_refreshsub(shtctl, vx0, vy0, vx1, vy1, height + 1, old);
    }
    return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
    struct SHTCTL *shtctl = sht->shtctl;
    if (sht->height >= 0)
    {
        sheet_refreshsub(shtctl, sht->vx0 + bx0, sht->vy0 + by0, \
                         sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
    }
    return;
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
    int oldx = sht->vx0;
    int oldy = sht->vy0;
    struct SHTCTL *shtctl = sht->shtctl;

    sht->vx0 = vx0;
    sht->vy0 = vy0;
    if (sht->height >= 0)
    {
        sheet_refreshmap(shtctl, oldx, oldy, oldx + sht->bxsize, oldy + sht->bysize, 0);
        sheet_refreshmap(shtctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
        sheet_refreshsub(shtctl, oldx, oldy, \
                         oldx + sht->bxsize, oldy + sht->bysize, 0, sht->height);
        sheet_refreshsub(shtctl, vx0,  vy0, \
                         vx0 + sht->bxsize,  vy0 + sht->bysize,  sht->height, sht->height);
    }

    return;
}

void sheet_free(struct SHEET *sht)
{
    if (sht->height >= 0)
        sheet_updown(sht, -1);
    sht->flags  = 0;
    return;
}
