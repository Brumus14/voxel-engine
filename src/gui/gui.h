#ifndef GUI_H
#define GUI_H

#include "../graphics/window.h"
#include "../graphics/shader_program.h"
#include "gui_element.h"
#include "image.h"

struct gui {
    struct gui_element *elements;
    int element_count;
    struct window *window;
    struct shader_program shader_program;
    int gl_orthographic_matrix_location;
};

void gui_init(struct gui *gui, struct window *window);
void gui_draw(struct gui *gui);
void gui_add_image(struct gui *gui, struct gui_image *image);

#endif
