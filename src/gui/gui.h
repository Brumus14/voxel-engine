#ifndef GUI_GUI_H
#define GUI_GUI_H

#include "../graphics/window.h"
#include "../graphics/shader_program.h"
#include "element.h"
#include "image.h"

struct gui {
    struct gui_element *elements;
    int element_count;
    struct window *window;
    struct shader_program shader_program;
    // Shouldn't this be perspective?
    int gl_orthographic_matrix_location;
};

void gui_init(struct gui *gui, struct window *window);
void gui_destroy(struct gui *gui);
void gui_draw(struct gui *gui);
// Maybe just add element
// Remove elements by having element ids
void gui_add_image(struct gui *gui, struct gui_image *image);

#endif
