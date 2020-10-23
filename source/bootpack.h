/* asmhead.nax */
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

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_out8(int add, int data);
void io_out16(int add, int data);
void io_out32(int add, int data);
int io_in8(int add);
int io_in16(int add);
int io_in32(int add);
int io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

/* graphic.c */
#define BLACK           0   /* 000000 */
#define BRIGHT_RED      1   /* FF0000 */
#define BRIGHT_GREEN    2   /* 00FF00 */
#define BRIGHT_YELLOW   3   /* FFFF00 */
#define BRIGHT_BLUE     4   /* 0000FF */
#define BRIGHT_PURPLE   5   /* FF00FF */
#define LIGHT_BLUE      6   /* 00FFFF */
#define WHITE           7   /* FFFFFF */
#define BRIGHT_GRAY     8   /* C6C6C6 */
#define DARK_RED        9   /* 840000 */
#define DARK_GREEN      10  /* 008400 */
#define DARK_YELLOW     11  /* 848400 */
#define DARK_BLUE       12  /* 000084 */
#define DARK_PURPLE     13  /* 840084 */
#define LIGHT_DARK_BLUE 14  /* 008484 */
#define DARK_GRAY       15  /* 848484 */
#define BACK_COLOR      BLACK

enum LineNum
{
    LN_MOUSE,
    LN_KEYBOARD,
};

void init_palette(void);
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0, int xlen, int ylen);
void init_screen8(char *vram, int xsize, int ysize); 
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
void init_mouse_cursor8(char *mouse, char bc);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void show_logo8(char *vram, int xsize, int x, int y, char c, char bc, unsigned char *logo, int psize);
void show_line8(char *vram, int xsize, enum LineNum line_num, unsigned char *msg);


/* dsctbl.c */
#define ADR_IDT         0x0026f800
#define LIMIT_IDT       0x000007ff
#define ADR_GDT         0x00270000
#define LIMIT_GDT       0x0000ffff
#define ADR_BOTPAK      0x00280000
#define LIMIT_BOTPAK    0x0007ffff
#define AR_DATA32_RW    0x4092
#define AR_CODE32_ER    0x409a
#define AR_INTGATE32    0x008e

struct SEGMENT_DESCRIPTOR
{
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR
{
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);


/* int.c */
#define PIC0_ICW1       0x0020
#define PIC0_OCW2       0x0020
#define PIC0_IMR        0x0021
#define PIC0_ICW2       0x0021
#define PIC0_ICW3       0x0021
#define PIC0_ICW4       0x0021
#define PIC1_ICW1       0x00a0
#define PIC1_OCW2       0x00a0
#define PIC1_IMR        0x00a1
#define PIC1_ICW2       0x00a1
#define PIC1_ICW3       0x00a1
#define PIC1_ICW4       0x00a1

#define PORT_KEYDAT             0x0060
#define PORT_KEYSTA             0x0064
#define PORT_KEYCMD             0x0064
#define KEYSTA_SEND_NOTREADY    0X02
#define KBC_MODE                0x47
#define KEYCMD_WRITE_MODE       0x60
#define KEYCMD_SENDTO_MOUSE     0xd4
#define MOUSECMD_ENABLE         0xf4

struct KEYBUF
{
    unsigned char data[32];
    int next;
};

void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);

/* fifo.c */
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