#include "gui.h"

#include "cglm/cglm.h"
#include "../util/gl.h"
#include "element.h"
#include "image.h"

void gui_init(struct gui *gui, struct window *window) {
    gui->window = window;
    gui->element_count = 0;
    gui->elements = NULL;

    shader_program_from_files(&gui->shader_program, "res/shaders/gui.vert",
                              "res/shaders/gui.frag");
    shader_program_link(&gui->shader_program);

    gui->gl_orthographic_matrix_location = GL_CALL_R(
        glGetUniformLocation(gui->shader_program.gl_id, "orthographic"), GLint);
}

void gui_destroy(struct gui *gui) {
    shader_program_destroy(&gui->shader_program);

    for (int i = 0; i < gui->element_count; i++) {
        gui_element_destroy(&gui->elements[i]);
    }

    free(gui->elements);
}

void gui_update_matrix_uniform(struct gui *gui) {
    mat4 glm_orthographic;
    glm_ortho(0, gui->window->width, gui->window->height, 0, -1, 1,
              glm_orthographic);

    GL_CALL(glUniformMatrix4fv(gui->gl_orthographic_matrix_location, 1,
                               GL_FALSE, glm_orthographic[0]));
}

void gui_draw(struct gui *gui) {
    shader_program_use(&gui->shader_program);
    gui_update_matrix_uniform(gui);

    for (int i = 0; i < gui->element_count; i++) {
        switch (gui->elements[i].type) {
        case GUI_ELEMENT_TYPE_IMAGE:
            gui_image_draw(gui->elements[i].data);
            break;
        }
    }
}

void gui_add_element(struct gui *gui, struct gui_element element) {
    gui->elements = realloc(gui->elements, (gui->element_count + 1) *
                                               sizeof(struct gui_element));
    gui->elements[gui->element_count] = element;
    gui->element_count++;
}

void gui_add_image(struct gui *gui, struct gui_image *image) {
    struct gui_element element;

    element.data = image;
    element.type = GUI_ELEMENT_TYPE_IMAGE;

    gui_add_element(gui, element);
}
