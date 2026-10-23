#include "pause_gui.h"

void pause_gui_init(struct pause_gui *gui, struct window *window) {
    gui_init(&gui->gui, window);

    gui_image_init(&gui->crosshair, "res/textures/pause_text.png", VEC2D_ZERO,
                   (struct vec2d){8, 8}, GUI_ELEMENT_ORIGIN_CENTER_CENTER,
                   GUI_ELEMENT_LAYER_1);

    gui_add_image(&gui->gui, &gui->crosshair);

    // gui_image_init(&gui->fade, "res/textures/pause_fade.png", VEC2D_ZERO,
    //                VEC2D_ONE, GUI_ELEMENT_ORIGIN_LEFT_TOP,
    //                GUI_ELEMENT_LAYER_2);
    //
    // gui_add_image(&gui->gui, &gui->fade);
}

void pause_gui_update(struct pause_gui *gui) {
    gui_image_set_position(&gui->crosshair,
                           (struct vec2d){(double)gui->gui.window->width / 2,
                                          (double)gui->gui.window->height / 2});

    // gui_image_set_scale(&gui->fade, (struct vec2d){gui->gui.window->width,
    //                                                gui->gui.window->height});
}

void pause_gui_draw(struct pause_gui *gui) {
    gui_draw(&gui->gui);
}

void pause_gui_destroy(struct pause_gui *gui) {
    gui_image_destroy(&gui->crosshair);
    // gui_image_destroy(&gui->fade);
}
