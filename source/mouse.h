#ifndef _MOUSE_H_
#define _MOUSE_H_

struct MOUSE_DEC
{
    unsigned char buf[3];
    unsigned char phase;
    int x, y;
    int btn;
};

void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
void init_mouse_cursor8(char *mouse, char bc);

#endif
