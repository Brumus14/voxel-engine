#include "hotbar.h"

void hotbar_init(struct hotbar *hotbar, struct gui *gui) {
    hotbar->current_slot = 0;

    tilemap_init(&hotbar->item_tilemap, "res/textures/icons.png",
                 TEXTURE_FILTER_NEAREST, 32, 32, 1, 2);

    for (int i = 0; i < 9; i++) {
        hotbar->items[i] = ITEM_TYPE_EMPTY;
    }

    hotbar->gui = gui;

    // multiply scale by the size
    gui_image_init(&hotbar->hotbar_image, "res/textures/hotbar.png", VEC2D_ZERO,
                   (struct vec2d){2, 2}, GUI_ELEMENT_ORIGIN_CENTER_BOTTOM,
                   GUI_ELEMENT_LAYER_0);

    for (int i = 0; i < 9; i++) {
        gui_image_init(&hotbar->item_images[i], "res/textures/icons.png",
                       VEC2D_ZERO, (struct vec2d){2, 2},
                       GUI_ELEMENT_ORIGIN_CENTER_BOTTOM, GUI_ELEMENT_LAYER_2);
    }

    gui_image_init(&hotbar->hotbar_selected_image,
                   "res/textures/hotbar_selected.png", VEC2D_ZERO,
                   (struct vec2d){2, 2}, GUI_ELEMENT_ORIGIN_CENTER_BOTTOM,
                   GUI_ELEMENT_LAYER_1);

    hotbar_update_gui(hotbar);

    gui_add_image(gui, &hotbar->hotbar_image);

    for (int i = 0; i < 9; i++) {
        gui_add_image(gui, &hotbar->item_images[i]);
    }

    gui_add_image(gui, &hotbar->hotbar_selected_image);
}

// Remove gui elements from gui
void hotbar_destroy(struct hotbar *hotbar) {
    tilemap_destroy(&hotbar->item_tilemap);
}

enum item_type hotbar_get_current_item(struct hotbar *hotbar) {
    return hotbar->items[hotbar->current_slot];
}

void hotbar_set_item(struct hotbar *hotbar, int slot, enum item_type item) {
    hotbar->items[slot] = item;
    hotbar_update_gui(hotbar);
}

// separate window resizing stuff
void hotbar_update_gui(struct hotbar *hotbar) {
    gui_image_set_position(
        &hotbar->hotbar_image,
        (struct vec2d){round((double)hotbar->gui->window->width / 2),
                       hotbar->gui->window->height - 2 * 2});

    for (int i = 0; i < 9; i++) {
        if (hotbar->items[i] == ITEM_TYPE_EMPTY) {
            hotbar->item_images[i].visible = false;

            continue;
        }

        hotbar->item_images[i].visible = true;

        double x =
            round((double)hotbar->gui->window->width / 2) + ((i - 4) * 34 * 2);

        gui_image_set_position(
            &hotbar->item_images[i],
            (struct vec2d){x, hotbar->gui->window->height - 4 * 2});

        gui_image_set_texture_rectangle(
            &hotbar->item_images[i],
            tilemap_get_tile_rectangle(
                &hotbar->item_tilemap,
                item_type_to_texture_index(hotbar->items[i])));
    }

    double selected_x = round((double)hotbar->gui->window->width / 2 +
                              (hotbar->current_slot - 4) * 34 * 2);

    gui_image_set_position(
        &hotbar->hotbar_selected_image,
        (struct vec2d){selected_x, hotbar->gui->window->height - 2 * 2});
}
