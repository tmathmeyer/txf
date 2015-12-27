#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdio.h>

#include "txf.h"
#include "txf_draw.h"

void onCreate(WINDOW win) {
    XL_WindowBackground(win, "#ccffcc");   
}

void record_onclick() {

}

void record_colors(BUTTON b, GRAPHICS g) {
    b->color = getcolor(g, "#f00");
}


void settings_onclick() {

}

void settings_colors(BUTTON b, GRAPHICS g) {
    b->color = getcolor(g, "#0f0");
}

int main() {
    WINDOW win = XL_WindowCreate(
            onCreate, 300, 300, 100, 100, NORM_WINDOW);

    XL_PanelSplitCreate(XL_WindowPanel(win), XL_HORIZONTAL, XL_FIXED, -50);
    ELEMENT spl =
        XL_PanelSplitCreate(
                XL_PanelSplitBottom(win->element),
                XL_VERTICAL,
                XL_FIXED,
                50);

    XL_ButtonCreate(
        XL_PanelSplitLeft(spl),
        record_colors,
        record_onclick);

    XL_ButtonCreate(
        XL_PanelSplitRight(spl),
        settings_colors,
        settings_onclick);
    
    XL_WaitOnWindow(win);
}
