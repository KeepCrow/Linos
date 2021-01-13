#ifndef _FIFO_H_
#define _FIFO_H_

#define FLAG_OVREFLOW       0x0001

struct FIFO8
{
    unsigned char *buf; /* buf地址 */
    int flag;   /* 是否溢出 */
    int size;   /* buf大小(字节) */
    int free;   /* 剩余字节数 */
    int r;  /* 读写下标 */
    int w;
};

/* 使用大小为size的buf初始fifo */
void fifo8_init(struct FIFO8 *fifo, unsigned char *buf, int size);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
/* 已使用 */
int fifo8_status(struct FIFO8 *fifo);

#endif