#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdio.h>

#include "txf.h"
#include "txf_draw.h"

void draw(WINDOW win, XWindowAttributes xwa) {
    unlong colorR = getcolor(win, "#4000");
    int x = xwa.width/2;
    int y = xwa.height/2;
    draw_rectangle(win, x/2, y/2, x, y, 1, colorR);
}

void event(WINDOW win, XEvent xe) {
    (void) win;
    (void) xe;
}

void onCreate(WINDOW win) {
    XL_WindowBackground(win, "#ccffcc");   
}

int main() {
    WINDOW win = XL_WindowCreate(
            onCreate, draw, event,
            300, 300, 100, 100, NORM_WINDOW);
    
    XL_WaitOnWindow(win);
}
