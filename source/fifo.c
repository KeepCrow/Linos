#include <stdio.h>
#include "bootpack.h"

int safe_add(int cur, int limit)
{
    return (cur + 1 >= limit) ? 0 : (cur + 1);
}

void fifo8_init(struct FIFO8 *fifo, unsigned char *buf, int size)
{
    fifo->buf = buf;
    fifo->r = fifo->w = 0;
    fifo->size = size;
    fifo->flag = 0;
    fifo->free = size;
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

int fifo8_status(struct FIFO8 *fifo)
{
    return fifo->size - fifo->free;
}