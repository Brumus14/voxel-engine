#include "main_gui.h"

#include "../math/math_util.h"

void main_gui_init(struct main_gui *gui, struct window *window) {
    gui_init(&gui->gui, window);

    gui_image_init(&gui->crosshair, "res/textures/crosshair.png", VEC2D_ZERO,
                   (struct vec2d){1, 1}, GUI_ELEMENT_ORIGIN_CENTER_CENTER,
                   GUI_ELEMENT_LAYER_0);

    gui_add_image(&gui->gui, &gui->crosshair);

    hotbar_init(&gui->hotbar, &gui->gui);

    hotbar_set_item(&gui->hotbar, 0, ITEM_TYPE_GRASS_BLOCK);
    hotbar_set_item(&gui->hotbar, 1, ITEM_TYPE_DIRT_BLOCK);
    hotbar_set_item(&gui->hotbar, 2, ITEM_TYPE_STONE_BLOCK);
    hotbar_set_item(&gui->hotbar, 3, ITEM_TYPE_COAL_BLOCK);
    hotbar_set_item(&gui->hotbar, 4, ITEM_TYPE_LOG_BLOCK);
    hotbar_set_item(&gui->hotbar, 5, ITEM_TYPE_DIAMOND_BLOCK);
    hotbar_set_item(&gui->hotbar, 6, ITEM_TYPE_LEAF_BLOCK);
}

void main_gui_update(struct main_gui *gui) {
    gui_image_set_position(
        &gui->crosshair,
        (struct vec2d){(double)gui->gui.window->width / 2,
                       (double)gui->gui.window->height /
                           2}); // only run when struct window size changed

    hotbar_update(&gui->hotbar, &gui->gui.window->keyboard,
                  &gui->gui.window->mouse);
}

void main_gui_draw(struct main_gui *gui) {
    gui_draw(&gui->gui);
}

void main_gui_destroy(struct main_gui *gui) {
    gui_image_destroy(&gui->crosshair);
    hotbar_destroy(&gui->hotbar);
}
