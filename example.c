#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdio.h>

#include "txf.h"
#include "txf_draw.h"


void record_onclick(BUTTON b) {
    (void)b;
    puts("record");
}

void settings_onclick(BUTTON b) {
    (void)b;
    puts("settings");
}

void onCreate(WINDOW win) {
    XL_PanelSplitCreate(XL_WindowPanel(win), XL_HORIZONTAL, XL_FIXED, -50);
    ELEMENT spl =
        XL_PanelSplitCreate(
                XL_PanelSplitBottom(win->element),
                XL_VERTICAL,
                XL_FIXED,
                50);

    XL_ButtonCreate(
        XL_PanelSplitLeft(spl),
        record_onclick);

    XL_ButtonCreate(
        XL_PanelSplitRight(spl),
        settings_onclick);
 
    XL_WindowForceRedraw(win);
}

int main() {
    WINDOW win = XL_WindowCreate(
            onCreate, 300, 300, 100, 100, NORM_WINDOW);

    XL_WaitOnWindow(win);
}
