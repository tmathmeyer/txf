#include "txf.h"
#include <stdio.h>

void XL_WindowRedraw(WINDOW win) {
    XMoveResizeWindow(
            win->disp
            ,win->win
            ,100
            ,100
            ,win->W
            ,win->H);
}



void XL_WindowResize(WINDOW win) {
    if(win->canvas) {
        XFreePixmap(win->disp, win->canvas);
    }
    win->canvas = XCreatePixmap(win->disp
            ,DefaultRootWindow(win->disp)
            ,win->W
            ,win->H
            ,win->depth);
}

void *run(void *v_win) {
    WINDOW win = (WINDOW)v_win;
    pthread_mutex_init(&win->lock, NULL);
    XEvent xe;
    while(1){
        pthread_mutex_lock(&win->lock);
        XNextEvent(win->disp, &xe);
        if (win->event) {
            (win->event)(win, xe);
        }
        XWindowAttributes xwa;
        if(XGetWindowAttributes(win->disp, win->win, &xwa)) {
            if (win->redraw) {
                (win->redraw)(win, xwa);
            }
            if (win->background) {
                
            }
        }
        pthread_mutex_unlock(&win->lock);
    }
    return NULL;
}

void XL_WindowBackground(WINDOW win, char *color) {
    win->background = getcolor(win, color);
}

WINDOW XL_WindowCreate(
        PAINT(draw), EVENT(ev),
        unint X, unint Y, unint W, unint H,
        unlong flags) {
    WINDOW res = calloc(sizeof(_txf_window), 1);
    res->W = W;
    res->H = H;
    res->redraw = draw;
    res->event = ev;

    /* create display and get root window */
    res->disp = XOpenDisplay(NULL);
    int screen = DefaultScreen(res->disp);
    Window root = RootWindow(res->disp, screen);
    
    /* set to allow transparency */
    XVisualInfo vinfo;
    XMatchVisualInfo(res->disp, DefaultScreen(res->disp), 32, TrueColor, &vinfo);
    unlong window_flags =
        CWEventMask|CWColormap|CWBorderPixel|CWBackPixel;
    res->wa.colormap =
        XCreateColormap(res->disp,DefaultRootWindow(res->disp),vinfo.visual,AllocNone);
    res->wa.border_pixel = 0;
    res->wa.background_pixel = 0;
    res->wa.override_redirect = True;
    res->wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    res->win = XCreateWindow(res->disp, root, X, Y, W, H, 0,
            vinfo.depth, InputOutput, vinfo.visual, window_flags, &res->wa);
    res->gc = XCreateGC(res->disp, res->win, 0, NULL);
    res->depth = vinfo.depth;
    XL_WindowResize(res);

    if (flags & DOCK_WINDOW) {
        /* set window in dock mode, so that WM wont fuck with it */
        long pval = XInternAtom (res->disp, "_NET_WM_WINDOW_TYPE_DOCK", False);
        long prop = XInternAtom (res->disp, "_NET_WM_WINDOW_TYPE", False);
        XChangeProperty(
                res->disp
                ,res->win
                ,prop
                ,XA_ATOM
                ,32
                ,PropModeReplace
                ,(unsigned char*)&pval
                ,1);
    }

    /* reserve space on the screen */
    long prop = XInternAtom(res->disp, "_NET_WM_STRUT_PARTIAL", False);
    long ptyp = XInternAtom(res->disp, "CARDINAL", False);
    int16_t strut[12] = {0, 0, H, 0, 0, 0, 0, 0, 0, W, 0, 0}; //+y after H
    XChangeProperty(
            res->disp
            ,res->win
            ,prop
            ,ptyp
            ,16
            ,PropModeReplace
            ,(unsigned char*)strut
            ,12);

    /* This is for support with legacy WMs */
    prop = XInternAtom(res->disp, "_NET_WM_STRUT", False);
    unsigned long strut_s[4] = {0, H, 0, 0};
    XChangeProperty(
            res->disp
            ,res->win
            ,prop
            ,ptyp
            ,32
            ,PropModeReplace
            ,(unsigned char*)strut_s
            ,4);

    if ((flags&OMNIPRESENT)&&(flags&DOCK_WINDOW)) {
        /* Appear on all desktops */
        prop = XInternAtom(res->disp, "_NET_WM_DESKTOP", False);
        long adsk = 0xffffffff;
        XChangeProperty(
                res->disp
                ,res->win
                ,prop
                ,ptyp
                ,32
                ,PropModeReplace
                ,(unsigned char*)&adsk
                ,1);

    }
    XSelectInput(res->disp, res->win, ExposureMask);
    
    res->thread = calloc(sizeof(pthread_t), 1);
    pthread_create(res->thread, NULL, run, res);
    
    XMapRaised(res->disp, res->win);
    return res;
}

void XL_WaitOnWindow(WINDOW win) {
    pthread_join(*win->thread, NULL);
}
