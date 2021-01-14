#ifndef _MEMORY_H_
#define _MEMORY_H_

/* 设置为4090后，memman的总大小为 4090 * (4 + 4) + 4 * 4 = 0x7fe0
 * 距离32KB(0x8000)仅差了0x0010也就是16个字节，2个freeinfo数据结构的大小
 * 那为什么不直接设为4092呢？
 */
#define MEMMAN_FREES    4090
#define MEMMAN_ADR      0x003c0000 

struct FREEINFO
{
    unsigned int addr, size;
};

struct MEMMAN
{
    /* 可用内存块，最大内存块，释放失败的内存总容量，释放失败的次数 */
    unsigned int frees, maxfrees, lostsize, losts;
    struct FREEINFO free[MEMMAN_FREES];
};

void memman_init(struct MEMMAN *man);

unsigned int memman_total(struct MEMMAN *man);

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);

unsigned int memman_alloc4k(struct MEMMAN *man, unsigned int size);

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

int memman_free4k(struct MEMMAN *man, unsigned addr, unsigned int size);

unsigned int memtest(unsigned int start, unsigned int end);

#endif
