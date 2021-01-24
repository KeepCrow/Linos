#ifndef _WINDOW_H_
#define _WINDOW_H_

#define TITLE_HEIGHT 20

struct WINDOW
{
    unsigned char title_color, body_color;
    char *title;
    unsigned char *buf;
    int xsize, ysize;
};

void init_window8(struct WINDOW *win, unsigned char *buf, char *title, int xsize, int ysize, unsigned char tc, unsigned bc);
void make_window8(struct WINDOW *win);

#endif
