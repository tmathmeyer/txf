

#ifndef _TXF_H_
#define _TXF_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <pthread.h>

#define unint unsigned int
#define unlong unsigned long

#define WINDOW struct _txf_window *
#define ELEMENT struct _txf_tree *
#define BUTTON struct _txf_button *
#define SPLIT struct _txf_split *
#define GRAPHICS struct _txf_grctx *

#define BUTTON_ID 2
#define SPLIT_ID 1

#define PAINT(x) void (*x)(struct _txf_window*, XWindowAttributes xwa)
#define EVENT(x) void (*x)(struct _txf_window*, XEvent xe)
#define WINFN(x) void (*x)(struct _txf_window*)

#define DOCK_WINDOW 0x80
#define OMNIPRESENT 0x40
#define FIXED_POSIT 0x20
#define NORM_WINDOW 0x00

#define XL_VERTICAL 0x01
#define XL_HORIZONTAL 0x02
#define XL_FIXED 0x04
#define XL_DYNAMIC 0x08

typedef struct _txf_grctx {
    Window canvas;
    Display *disp;
    GC gc;
    Colormap map;
    unint offsetX;
    unint offsetY;
    unint width;
    unint height;
} _txf_grctx;

typedef struct _txf_window {
    Display *disp;
    Pixmap canvas;
    XSetWindowAttributes wa;
    Window win;
    GC gc;
    unint depth;
    pthread_mutex_t lock;
    pthread_t *thread;
    WINFN(init);
    unint W;
    unint H;
    unlong background;
    ELEMENT element;
} _txf_window;

typedef struct _txf_split {
    unint id;
    unint split_orient;
    unint split_sizing;
    float split_value;
    ELEMENT top_or_left;
    ELEMENT bot_or_right;
} _txf_split;

typedef struct _txf_button {
    unint id;
    unlong color;
    void (*init)(BUTTON, GRAPHICS);
    void (*click)(BUTTON);
} _txf_button;

typedef struct _txf_tree {
    union {
        struct {
            unint id;
        };
        struct _txf_split split;
        struct _txf_button button;
    };
} _txf_tree;

WINDOW XL_WindowCreate(
        WINFN(init),
        unint X, unint Y, unint W, unint H,
        unlong flags);
void XL_WindowResize(WINDOW);
void XL_WindowRedraw(WINDOW);
void XL_WaitOnWindow(WINDOW);

ELEMENT XL_PanelSplitCreate(ELEMENT *, unint, unint, float);
ELEMENT XL_ButtonCreate(ELEMENT *, void(*)(BUTTON, GRAPHICS), void(*)(BUTTON));

ELEMENT *XL_PanelSplitTop(ELEMENT);
ELEMENT *XL_PanelSplitLeft(ELEMENT);
ELEMENT *XL_PanelSplitBottom(ELEMENT);
ELEMENT *XL_PanelSplitRight(ELEMENT);
ELEMENT *XL_WindowPanel(WINDOW);

void _txf_draw(ELEMENT e, GRAPHICS g);

GRAPHICS defaultGraphics(WINDOW win);

#endif
