#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include "../gui/gui.h"
#include "../gui/hotbar.h"

struct main_gui {
    struct gui gui;
    struct gui_image crosshair;
    struct hotbar hotbar;
};

void main_gui_init(struct main_gui *gui);
void main_gui_update(struct main_gui *gui);
void main_gui_destroy(struct main_gui *gui);

#endif
