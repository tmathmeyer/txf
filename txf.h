

#ifndef _TXF_H_
#define _TXF_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <pthread.h>

#define unint unsigned int
#define unlong unsigned long

#define WINDOW _txf_window *
#define PAINT(x) void (*x)(struct _txf_window*, XWindowAttributes xwa)
#define EVENT(x) void (*x)(struct _txf_window*, XEvent xe)
#define WINFN(x) void (*x)(struct _txf_window*)
#define DOCK_WINDOW 0x80
#define OMNIPRESENT 0x40
#define FIXED_POSIT 0x20
#define NORM_WINDOW 0x00

typedef struct _txf_window{
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
} _txf_window;

WINDOW XL_WindowCreate(
        WINFN(init), PAINT(draw), EVENT(ev),
        unint X, unint Y, unint W, unint H,
        unlong flags);
void XL_WindowResize(WINDOW);
void XL_WindowRedraw(WINDOW);
void XL_WaitOnWindow(WINDOW);

#endif
