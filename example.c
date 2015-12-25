#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdio.h>

#include "txf.h"
#include "txf_draw.h"

#define WIDTH 300
#define HEIGHT 300
#define X 100
#define Y 100
#define border 6

void draw(WINDOW win, XWindowAttributes xwa) {
    unlong colorR = getcolor(win, "#ccffcc");
    draw_rectangle(win, 0, 0, 100, 150, 1, colorR);
    printf("fug\n");
}

void event(WINDOW win, XEvent xe) {
    
    printf("event\n");
}

int main() {
    WINDOW win = XL_WindowCreate(draw, event, 300, 300, 100, 100, NORM_WINDOW);
    XL_WindowBackground(win, "#ccffcc");   
    
    XL_WaitOnWindow(win);
}
