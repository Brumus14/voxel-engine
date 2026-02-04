#ifndef GUI_HOTBAR_H
#define GUI_HOTBAR_H

#include "gui.h"
#include "image.h"
#include "../graphics/tilemap.h"
#include "../game/items.h"

// Don't repeat texture in each item image instead share
struct hotbar {
    enum item_type items[9];
    int current_slot;
    struct tilemap item_tilemap;
    struct gui *gui;
    struct gui_image hotbar_image;
    struct gui_image item_images[9];
    struct gui_image hotbar_selected_image;
};

void hotbar_init(struct hotbar *hotbar, struct gui *gui);
void hotbar_destroy(struct hotbar *hotbar);
enum item_type hotbar_get_current_item(struct hotbar *hotbar);
void hotbar_set_item(struct hotbar *hotbar, int slot, enum item_type item);
void hotbar_update_gui(struct hotbar *hotbar);

#endif
