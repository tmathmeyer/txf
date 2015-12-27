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
unsigned long color_in_range(GRAPHICS, char *, char *, int);
unsigned long _getcolor(GRAPHICS, const char *);
unsigned long alphaset(ulng, uint8_t);
unsigned long getcolor(GRAPHICS, const char *);
void draw_rectangle(GRAPHICS, XYWH, char, ulng);

void XL_WindowBackground(WINDOW, char *);
#endif
