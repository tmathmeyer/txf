

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

typedef struct _txf_split {
    unint id;
    unint split_orient;
    unint split_sizing;
    unint split_value;
    struct _txf_tree *top_or_left;
    struct _txf_tree *bot_or_right;
} _txf_split;

typedef struct _txf_button {
    unint id;
    char *text;
    void (*init)(struct _txf_button *);
    void (*click)(struct _txf_button *);
} _txf_button;

typedef struct _txf_window {
    Display *disp;
    Pixmap canvas;
    XSetWindowAttributes wa;
    Window win;
    GC gc;
    unint depth;
    pthread_mutex_t lock;
    pthread_t *thread;
    PAINT(redraw);
    WINFN(init);
    EVENT(event);
    unint W;
    unint H;
    unlong background;
    ELEMENT element;
} _txf_window;

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
        WINFN(init), PAINT(draw), EVENT(ev),
        unint X, unint Y, unint W, unint H,
        unlong flags);
void XL_WindowResize(WINDOW);
void XL_WindowRedraw(WINDOW);
void XL_WaitOnWindow(WINDOW);

void XL_PanelSplitCreate(ELEMENT *, unint, unint, float);
void XL_ButtonCreate(ELEMENT *, void(*)(BUTTON), void(*)(BUTTON));

ELEMENT *XL_PanelSplitLeft(ELEMENT);
ELEMENT *XL_PanelSplitRight(ELEMENT);
ELEMENT *XL_WindowPanel(WINDOW);

#endif
