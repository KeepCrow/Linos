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
void io_out8(int add, int data);
int io_load_eflags();
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

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

void init_palette(void);
void boxfill8(char *vram, int xsize, unsigned char c, int x0, int y0, int xlen, int ylen);
void init_screen(char *vram, int xsize, int ysize); 
void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
void init_mouse_cursor8(char *mouse, char bc);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);


/* dsctbl.c */
#define ADR_IDT         0x0026f800
#define LIMIT_IDT       0x000007ff
#define ADR_GDT         0x00270000
#define LIMIT_GDT       0x0000ffff
#define ADR_BOTPAK      0x00280000
#define LIMIT_BOTPAK    0x0007ffff
#define AR_DATA32_RW    0x4092
#define AR_DATA32_ER    0x409a

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

void init_pic(void);
