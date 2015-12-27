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

void record_onclick() {

}

void record_colors() {

}


void settings_onclick() {

}

void settings_colors() {

}

int main() {
    WINDOW win = XL_WindowCreate(
            onCreate, draw, event,
            300, 300, 100, 100, NORM_WINDOW);
/*
    XL_PanelSplitCreate(XL_WindowPanel(win), XL_VERTICAL, XL_FIXED, -50);
    XL_ButtonCreate(
        XL_PanelSplitLeft(win->element),
        record_colors,
        record_onclick);

    XL_ButtonCreate(
        XL_PanelSplitRight(win->element),
        settings_colors,
        settings_onclick);
    
*/
    XL_WaitOnWindow(win);
}
