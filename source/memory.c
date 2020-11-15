#include "memory.h"

#define EFLAGS_AC_BIT       0x00040000
#define CR0_CACHE_DISABLE   0x60000000

int load_cr0();

void store_cr0(int cr0);

unsigned int memtest_sub(unsigned int start, unsigned int end);

void memman_init(struct MEMMAN *man)
{
    man->frees = man->losts = man->lostsize = man->maxfrees = 0;
}

unsigned int memman_total(struct MEMMAN *man)
{
    unsigned int i, total;
    for (i = 0; i < man->frees; i++)
        total += man->free[i].size;
    return total;
}

/**
 * @brief 将free列表中的下标为i的元素从列表中删除，其他元素前移一位
 * 
 * @param size free列表的总长度
 */
static void free_delete(struct MEMMAN *man, unsigned int i)
{
    for (i = i + 1; i < man->frees; i++)
        man->free[i - 1] = man->free[i];
    man->frees -= 1;
    return;
}

/* 更新内存管理中的内存块最大数量，并返回是否更新 */
int update_max(struct MEMMAN *man)
{
    if (man->maxfrees < man->frees)
        man->maxfrees = man->frees;
    return man->maxfrees == man->frees;
}

/**
 * @brief 向free列表中下标为i的元素后插入一个新元素，其他元素后移1位
 * 
 * @param fi 待插入的内存块信息
 */
static void free_insert(struct MEMMAN *man, unsigned int i, struct FREEINFO *fi)
{
    unsigned int j;
    for (j = man->frees; j > i; j--)
        man->free[j] = man->free[j - 1];
        
    man->free[i].addr = fi->addr;
    man->free[i].size = fi->size;
    man->frees += 1;
    update_max(man);
    return;
}

static void free_lost(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    man->losts += 1;
    man->lostsize +=1;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
    unsigned int i, addr;

    addr = 0;
    for (i = 0; i < man->frees; i++)
    {
        if (man->free[i].size >= size)
        {
            addr = man->free[i].addr;
            man->free[i].addr += size;
            man->free[i].size -= size;

            if (man->free[i].size == 0)
                free_delete(man, i);
        }
    }

    return addr;
}

/* 将两个内存块组合，成功返回1，失败返回0 */
static int fi_combine(struct FREEINFO *pre, struct FREEINFO *next)
{
    if (pre->addr + pre->size == next->addr)
    {
        pre->size += next->size;
        return 1;
    }
    return 0;
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
    int i = 0, res1, res2;
    struct FREEINFO fi = (struct FREEINFO){addr, size};

    /* 为便于归纳内存，将free[]按照addr的顺序排列，先决定应该放在哪里 */
    while (i < man->frees && man->free[i].addr > addr)
        i += 1;

    if (i > 0)
    {
        /* 尝试向前合并 */
        res1 = fi_combine(&(man->free[i-1]), &fi); 
        if (res1)
        {
            /* 如果成功，再尝试向后合并 */
            res2 = fi_combine(&(man->free[i-1]), &(man->free[i]));
            if (res2)
                free_delete(man, i);    /* 都成功则删除后者的内存块信息 */
            return 0;
        }
    }

    if (i < man->frees)
    {   
        /* 尝试向后合并 */
        res2 = fi_combine(&fi, &(man->free[i]));
        if (res2)
        {
            man->free[i] = fi;  /* 成功则将合并后的信息存储到后者 */
            return 0;
        }   
    } 

    /* 无法合并，直接插入 */
    if (man->frees < MEMMAN_FREES)
    {
        free_insert(man, i, &fi);
        return 0;
    }

    /* 无法插入 */
    free_lost(man, addr, size);
    return -1;
}

/* 判断CPU型号是否为486 */
static char is486(void)
{
    char flg486;
    unsigned int eflg;

    eflg = io_load_eflags();
    eflg |= EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    eflg = io_load_eflags();
    flg486 = (eflg & EFLAGS_AC_BIT != 0) ? 1 : 0;

    /* 恢复原本的eflags寄存器值 */
    eflg &= ~EFLAGS_AC_BIT;
    io_store_eflags(eflg);

    return flg486;
}

static void disable_cache()
{
    unsigned int cr0;
    cr0 = load_cr0();
    cr0 |= CR0_CACHE_DISABLE;
    store_cr0(cr0);
}

static void enable_cache()
{
    unsigned int cr0;
    cr0 = load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE;
    store_cr0(cr0);
}

unsigned int memtest(unsigned int start, unsigned int end)
{
    char flg486;
    unsigned int mem_size = 0;

    flg486 = is486();
    if (flg486 == 1)
        disable_cache();
    mem_size = memtest_sub(start, end);
    if (flg486 == 1)
        enable_cache();

    return mem_size;
}