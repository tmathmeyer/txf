#define _DEFAULT_SOURCE

#include "txf.h"
#include "txf_draw.h"

uint8_t hex(char c) {
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c-'0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(10+c-'a');
    }
    if (c >= 'A' && c <= 'F') {
        return (uint8_t)(10+c-'A');
    }
    return 0;
}

char unhex(uint8_t x) {
    if (x<=9) {
        return '0'+x;
    }
    if (x>=10 && x<=15) {
        return 'a'+x-10;
    }
    return -1;
}

unsigned long color_in_range(WINDOW win, char *clow, char *chigh, int percent) {
    int a_s = strlen(clow);
    int b_s = strlen(chigh);
    if (a_s != 7 || b_s != 7) {
        return 0;
    }
    char res[7] = {0};
    res[0] = '#';
    
    uint8_t a_red = 16*hex(clow[1])+hex(clow[2]);
    uint8_t a_grn = 16*hex(clow[3])+hex(clow[4]);
    uint8_t a_blu = 16*hex(clow[5])+hex(clow[6]);

    uint8_t b_red = 16*hex(chigh[1])+hex(chigh[2]);
    uint8_t b_grn = 16*hex(chigh[3])+hex(chigh[4]);
    uint8_t b_blu = 16*hex(chigh[5])+hex(chigh[6]);

    uint8_t n_red = a_red + ((b_red-a_red)*percent)/100;
    uint8_t n_grn = a_grn + ((b_grn-a_grn)*percent)/100;
    uint8_t n_blu = a_blu + ((b_blu-a_blu)*percent)/100;

    res[1] = unhex(n_red/16);
    res[2] = unhex(n_red%16);
    res[3] = unhex(n_grn/16);
    res[4] = unhex(n_grn%16);
    res[5] = unhex(n_blu/16);
    res[6] = unhex(n_blu%16);

    unsigned long result = getcolor(win, res);
    return result;
}

unsigned long _getcolor(WINDOW win, const char *colstr) {
    XColor color;
    if(!XAllocNamedColor(win->disp, win->wa.colormap, colstr, &color, &color)) {
        return 0;
    }
    return color.pixel;
}

unsigned long alphaset(ulng color, uint8_t alpha) {
    uint32_t mod = alpha;
    mod <<= 24;
    return (0x00ffffff & color) | mod;
}

unsigned long getcolor(WINDOW win, const char *colstr) {
    char *rgbcs = strdup(colstr);
    char *freeme = rgbcs;
    uint8_t value;
    switch(strlen(colstr)) {
        case 4: // #rgb
        case 7: // #rrggbb
            free(freeme);
            return _getcolor(win, colstr);
        case 9: // #aarrggbb
            value = 16*hex(colstr[1]) + hex(colstr[2]);
            rgbcs += 2;
            break;
        case 5: // #argb
            value = hex(colstr[1]) * 17;
            rgbcs += 1;
            break;
    }
    rgbcs[0] = '#';
    ulng result = _getcolor(win, rgbcs);
    result = alphaset(result, value);
    free(freeme);
    return result;
}


void draw_rectangle(WINDOW win, XYWH, char fill, ulng color) {
    XSetForeground(win->disp, win->gc, color);
    (fill?XFillRectangle:XDrawRectangle)(
            win->disp
            ,win->win
            ,win->gc
            ,X, Y, W, H);
}
