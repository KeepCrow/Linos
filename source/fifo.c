#include <stdio.h>
#include "fifo.h"

static inline int safe_add(int cur, int limit)
{
    return (cur + 1 >= limit) ? 0 : (cur + 1);
}

void fifo8_init(struct FIFO8 *fifo, unsigned char *buf, int size)
{
    fifo->buf  = buf;
    fifo->size = size;
    fifo->flag = 0;
    fifo->free = size;
    fifo->w    = 0;
    fifo->r    = 0;
}

void fifo32_init(struct FIFO32 *fifo, int *buf, int size)
{
    fifo->buf  = buf;
    fifo->size = size;
    fifo->flag = 0;
    fifo->free = size;
    fifo->w    = 0;
    fifo->r    = 0;
}

int fifo8_put(struct FIFO8 *fifo, unsigned char data)
{
    if (fifo->free == 0)
    {
        fifo->flag = FLAG_OVREFLOW;
        return -1;
    }

    if (fifo->buf == NULL)
    {
        return -1;
    }

    /* 填写数据，更新写下标，更新剩余空间 */
    fifo->buf[fifo->w] = data;
    fifo->w = safe_add(fifo->w, fifo->size);
    fifo->free -= 1;
    return 0;
}

int fifo32_put(struct FIFO32 *fifo, int data)
{
    if (fifo->free == 0)
    {
        fifo->flag = FLAG_OVREFLOW;
        return -1;
    }

    if (fifo->buf == NULL)
    {
        return -1;
    }

    /* 填写数据，更新写下标，更新剩余空间 */
    fifo->buf[fifo->w] = data;
    fifo->w = safe_add(fifo->w, fifo->size);
    fifo->free -= 1;
    return 0;
}

int fifo8_get(struct FIFO8 *fifo)
{
    int data;
    if (fifo->free == fifo->size)
        return -1;
    
    data = fifo->buf[fifo->r];
    fifo->r = safe_add(fifo->r, fifo->size);
    fifo->free += 1;
    return data;
}

int fifo32_get(struct FIFO32 *fifo)
{
    int data;
    if (fifo->free == fifo->size)
        return -1;
    
    data = fifo->buf[fifo->r];
    fifo->r = safe_add(fifo->r, fifo->size);
    fifo->free += 1;
    return data;
}

int fifo8_status(struct FIFO8 *fifo)
{
    return fifo->size - fifo->free;
}

int fifo32_status(struct FIFO32 *fifo)
{
    return fifo->size - fifo->free;
}

