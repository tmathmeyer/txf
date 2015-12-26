#ifndef _TXF_DRAW_H_
#define _TXF_DRAW_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define ulng unsigned long
#define XYWH ulng X, ulng Y, ulng W, ulng H

uint8_t hex(char c);
char unhex(uint8_t x);
unsigned long color_in_range(WINDOW win, char *clow, char *chigh, int percent);
unsigned long _getcolor(WINDOW win, const char *colstr);
unsigned long alphaset(ulng color, uint8_t alpha);
unsigned long getcolor(WINDOW win, const char *colstr);
void draw_rectangle(WINDOW win, XYWH, char fill, ulng color);

void XL_WindowBackground(WINDOW, char *);   
#endif
