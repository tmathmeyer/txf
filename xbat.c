/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <pwd.h>
#include "lwbi.h"

#define uchr unsigned char
#define unt unsigned int
#define ulng unsigned long

static void run(void);
static void resize(unt w, unt h);
ulng getcolor(const char *colstr);
static void setup(void);

static int width = 30;
static int height = 200;
static int border = 6;
static int x = 1840;
static int y = 1000;

unsigned long colorR;
unsigned long colorW;

static unsigned int depth;
static Window win;
static XSetWindowAttributes wa;
static Display *disp;
static GC gc;
static Pixmap canvas = NULL;
static char *bat_id = "BAT1";

int main(int argv, char **argc) {
    int c = 1;
    char *border_color = "#fff";
    while(c<argv-1) {
        if (!strcmp("-w", argc[c])) {
            width = atoi(argc[c+1]);
        }
        if (!strcmp("-bw", argc[c])) {
            border = atoi(argc[c+1]);
        }
        if (!strcmp("-x", argc[c])) {
            x = atoi(argc[c+1]);
        }
        if (!strcmp("-y", argc[c])) {
            y = atoi(argc[c+1]);
        }
        if (!strcmp("-bc", argc[c])) {
            border_color = argc[c+1];
        }
        if (!strcmp("-bat", argc[c])) {
            bat_id = argc[c+1];
        }
        c+=2;
    }

    setup();
    colorW = getcolor(border_color);
    run();
    return EXIT_FAILURE;
}

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
    printf("bad unhex!");
    exit(1);
}

ulng color_in_range(char *clow, char *chigh, int percent) {
    int a_s = strlen(clow);
    int b_s = strlen(chigh);
    if (a_s != 7 || b_s != 7) {
        puts("color range must be in the form #XXXXXX:#XXXXXX");
        exit(1);
    }
    char *res = calloc(sizeof(char), a_s);
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

    ulng result = getcolor(res);
    free(res);
    return result;
}

void remap(Window win, unsigned int w, unsigned int h) {
    XClearArea(disp, win, 0, 0, w, h, 0);
    XCopyArea(disp, canvas, win, gc, 0, 0, w, h, 0, 0);
    XFlush(disp);
}

ulng _getcolor(const char *colstr) {
    XColor color;
    if(!XAllocNamedColor(disp, wa.colormap, colstr, &color, &color)) {
        return 0;
    }
    return color.pixel;
}

ulng alphaset(ulng color, uint8_t alpha) {
    uint32_t mod = alpha;
    mod <<= 24;
    return (0x00ffffff & color) | mod;
}

ulng getcolor(const char *colstr) {
    char *rgbcs = strdup(colstr);
    char *freeme = rgbcs;
    uint8_t value;
    switch(strlen(colstr)) {
        case 4: // #rgb
        case 7: // #rrggbb
            free(freeme);
            return _getcolor(colstr);
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
    ulng result = _getcolor(rgbcs);
    result = alphaset(result, value);
    free(freeme);
    return result;
}


void draw_rectangle(ulng x, ulng y, ulng w, ulng h, Bool fill, ulng color) {
    XRectangle rect = {x, y, w, h};
    XSetForeground(disp, gc, color);
    (fill?XFillRectangles:XDrawRectangles)(disp, canvas, gc, &rect, 1);
}


void resize(unt w, unt h) {
    if(canvas) {
        XFreePixmap(disp, canvas);
    }
    canvas=XCreatePixmap(disp,DefaultRootWindow(disp),w,h,depth);
}

void run(void) {
    XEvent xe;
    while(1){
        while(QLength(disp)) {
            XNextEvent(disp, &xe);
        }
        int bat = get_battery_percent(bat_id);
        height = bat*3;
        resize(width, height);
        XMoveResizeWindow(disp, win, x-width, y-height, width, height);

        colorR = color_in_range("#ff0000", "#00ff00", bat);

        draw_rectangle(0,0,width,height,1,colorW);
        draw_rectangle(border,border,
                width-(2*border),height-(2*border),1,colorR);
        remap(win, width, height);
        usleep(60000000);
    }
}

void setup() {
    /* create display and get root window */
    disp = XOpenDisplay(NULL);
    int screen = DefaultScreen(disp);
    Window root = RootWindow(disp, screen);
    
    /* set to allow transparency */
    XVisualInfo vinfo;
    XMatchVisualInfo(disp, DefaultScreen(disp), 32, TrueColor, &vinfo);
    ulng window_flags =
        CWOverrideRedirect|CWEventMask|CWColormap|CWBorderPixel|CWBackPixel;
    wa.colormap =
        XCreateColormap(disp,DefaultRootWindow(disp),vinfo.visual,AllocNone);
    wa.border_pixel = 0;
    wa.background_pixel = 0;
    wa.override_redirect = True;
    wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    win = XCreateWindow(disp, root, x, y, width, height, 0,
            vinfo.depth, InputOutput, vinfo.visual, window_flags, &wa);
    gc = XCreateGC(disp, win, 0, NULL);
    depth = vinfo.depth;
    resize(width, height);
    XMapRaised(disp, win);

    /* set window in dock mode, so that WM wont fuck with it */
    long pval = XInternAtom (disp, "_NET_WM_WINDOW_TYPE_DOCK", False);
    long prop = XInternAtom (disp, "_NET_WM_WINDOW_TYPE", False);
    XChangeProperty(disp,win,prop,XA_ATOM,32,PropModeReplace,(uchr*)&pval,1);

    /* reserve space on the screen */
    prop = XInternAtom(disp, "_NET_WM_STRUT_PARTIAL", False);
    long ptyp = XInternAtom(disp, "CARDINAL", False);
    int16_t strut[12] = {0, 0, height+y, 0, 0, 0, 0, 0, 0, width, 0, 0};
    XChangeProperty(disp,win,prop,ptyp,16,PropModeReplace,(uchr*)strut,12);

    /* This is for support with legacy WMs */
    prop = XInternAtom(disp, "_NET_WM_STRUT", False);
    unsigned long strut_s[4] = {0, height, 0, 0};
    XChangeProperty(disp,win,prop,ptyp,32,PropModeReplace,(uchr*)strut_s,4);

    /* Appear on all desktops */
    prop = XInternAtom(disp, "_NET_WM_DESKTOP", False);
    long adsk = 0xffffffff;
    XChangeProperty(disp,win,prop,ptyp,32,PropModeReplace,(uchr*)&adsk,1);


    XSelectInput(disp, win, ExposureMask);
}
