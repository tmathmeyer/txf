#include "txf.h"
#include "txf_draw.h"
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
    XNextEvent(win->disp, &xe);
    if (win->init) {
        (win->init)(win);
    }
    while(1){
        pthread_mutex_lock(&win->lock);
        GRAPHICS g = defaultGraphics(win);
        if(g) {
            if (win->background) {
                draw_rectangle(g,0,0,g->width,g->height,1,win->background);
            }
            _txf_draw(win->element, g);
        }
        XNextEvent(win->disp, &xe);
        pthread_mutex_unlock(&win->lock);
    }
    return NULL;
}

WINDOW XL_WindowCreate(
        WINFN(init),
        unint X, unint Y, unint W, unint H,
        unlong flags) {
    WINDOW res = calloc(sizeof(_txf_window), 1);
    res->W = W;
    res->H = H;
    res->init = init;

    /* create display and get root window */
    res->disp = XOpenDisplay(NULL);
    int screen = DefaultScreen(res->disp);
    Window root = RootWindow(res->disp, screen);
    
    /* set to allow transparency */
    XVisualInfo vinfo;
    XMatchVisualInfo(res->disp,DefaultScreen(res->disp),32,TrueColor,&vinfo);
    unlong window_flags =
        CWEventMask|CWColormap|CWBorderPixel|CWBackPixel;
    res->wa.colormap =
        XCreateColormap(res->disp,DefaultRootWindow(res->disp),
                vinfo.visual,AllocNone);
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













ELEMENT createSplit(unint v_h, unint f_d, float pos) {
    SPLIT real = calloc(sizeof(struct _txf_split), 1);
    real->id = SPLIT_ID;

    real->split_orient = v_h;
    real->split_sizing = f_d;
    real->split_value = pos;

    real->top_or_left = NULL;
    real->bot_or_right = NULL;
    return (ELEMENT)real;
}

ELEMENT XL_PanelSplitCreate(ELEMENT *ele, unint v_h, unint f_d, float pos) {
    return (*ele = createSplit(v_h, f_d, pos));
}


ELEMENT XL_ButtonCreate(ELEMENT *ele, void(*init)(BUTTON, GRAPHICS), void(*click)(BUTTON)) {
    BUTTON b = calloc(sizeof(struct _txf_button), 1);
    b->id = BUTTON_ID;
    b->click = click;
    b->init = init;

    return ((*ele) = (ELEMENT)b);
}

ELEMENT *XL_PanelSplitLeft(ELEMENT ele) {
    if (ele->id != SPLIT_ID) {
        perror("called XL_PanelSplitLeft on a non split element");
        exit(1);
    }
    if (((SPLIT)ele)->split_orient != XL_VERTICAL) {
        perror("split is horizontal, cannot get left side");
        exit(1);
    }
    return &(((SPLIT)ele)->top_or_left);
}

ELEMENT *XL_PanelSplitRight(ELEMENT ele) {
    if (ele->id != SPLIT_ID) {
        perror("called XL_PanelSplitRight on a non split element");
        exit(1);
    }
    if (((SPLIT)ele)->split_orient != XL_VERTICAL) {
        perror("split is horizontal, cannot get right side");
        exit(1);
    }
    return &(((SPLIT)ele)->bot_or_right);
}

ELEMENT *XL_PanelSplitTop(ELEMENT ele) {
    if (ele->id != SPLIT_ID) {
        perror("called XL_PanelSplitTop on a non split element");
        exit(1);
    }
    if (((SPLIT)ele)->split_orient != XL_HORIZONTAL) {
        perror("split is vertical, cannot get top");
        exit(1);
    }
    return &(((SPLIT)ele)->top_or_left);
}

ELEMENT *XL_PanelSplitBottom(ELEMENT ele) {
    if (ele->id != SPLIT_ID) {
        perror("called XL_PanelSplitBot on a non split element");
        exit(1);
    }
    if (((SPLIT)ele)->split_orient != XL_HORIZONTAL) {
        perror("split is vertical, cannot get bot");
        exit(1);
    }
    return &(((SPLIT)ele)->bot_or_right);
}

ELEMENT *XL_WindowPanel(WINDOW win) {
    return &(win->element);
}



GRAPHICS mkgraphics(GRAPHICS g, unint x, unint y, unint w, unint h) {
    GRAPHICS res = calloc(sizeof(struct _txf_grctx), 1);
    res->offsetX = x;
    res->offsetY = y;
    res->width = w;
    res->height = h;
    res->canvas = g->canvas;
    res->disp = g->disp;
    res->gc = g->gc;
    res->map = g->map;
    return res;
}

void _txf_draw(ELEMENT e, GRAPHICS g) {
    if (e == NULL) {
        return;
    }
    switch(e->id) {
        case SPLIT_ID:
            (void)"Recursively call draw on the left and right halves";
            SPLIT s = (SPLIT)e;
            if (s->split_orient == XL_HORIZONTAL) {
                int topheight;
                if (s->split_sizing == XL_DYNAMIC) {
                    topheight = ((int)(g->height * s->split_value))/100;
                }
                if (s->split_sizing == XL_FIXED) {
                    if (s->split_value > 0) {
                        topheight = s->split_value;
                    } else {
                        topheight = s->split_value + g->height;
                    }
                }
                _txf_draw(*XL_PanelSplitTop(e), 
                        mkgraphics(
                            g,
                            g->offsetX,
                            g->offsetY,
                            g->width,
                            topheight));
                _txf_draw(*XL_PanelSplitBottom(e),
                        mkgraphics(
                            g,
                            g->offsetX,
                            g->offsetY+topheight,
                            g->width,
                            g->height-topheight));

            }
            if (s->split_orient == XL_VERTICAL) {
                int leftwidth;
                if (s->split_sizing == XL_DYNAMIC) {
                    leftwidth = ((int)(g->width * s->split_value))/100;
                }
                if (s->split_sizing == XL_FIXED) {
                    leftwidth = s->split_value>0 ? s->split_value
                                                 : s->split_value + g->width;
                }
                _txf_draw(*XL_PanelSplitLeft(e), 
                        mkgraphics(
                            g,
                            g->offsetX,
                            g->offsetY,
                            leftwidth,
                            g->height));
                _txf_draw(*XL_PanelSplitRight(e),
                        mkgraphics(
                            g,
                            g->offsetX+leftwidth,
                            g->offsetY,
                            g->width-leftwidth,
                            g->height));
            }
            break;
        case BUTTON_ID:
            (void)"draw a button";
            BUTTON b = (BUTTON)e;
            (b->init)(b, g);
            draw_rectangle(g, 0, 0, g->width, g->height, 1, b->color);
            break;
    }
    free(g);
}

GRAPHICS defaultGraphics(WINDOW win) {
    XWindowAttributes xwa;
    if(XGetWindowAttributes(win->disp, win->win, &xwa)) {
        GRAPHICS res = calloc(sizeof(struct _txf_grctx), 1);
        res->offsetX = 0;
        res->offsetY = 0;
        res->width = xwa.width;
        res->height = xwa.height;
        res->canvas = win->win;
        res->disp = win->disp;
        res->gc = win->gc;
        res->map = win->wa.colormap;
        return res;
    }
    return NULL;
}

