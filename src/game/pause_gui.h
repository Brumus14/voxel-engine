#ifndef PAUSE_GUI_H
#define PAUSE_GUI_H

#include "../gui/gui.h"

struct pause_gui {
    struct gui gui;
    struct gui_image crosshair;
    struct gui_image fade;
};

void pause_gui_init(struct pause_gui *gui, struct window *window);
void pause_gui_update(struct pause_gui *gui);
void pause_gui_draw(struct pause_gui *gui);
void pause_gui_destroy(struct pause_gui *gui);

#endif
