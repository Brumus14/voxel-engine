#include "image.h"

#include "../graphics/renderer.h"

// create function to set origin
void gui_image_update(struct gui_image *gui_image) {
    bo_bind(&gui_image->vbo);

    // add option to set image origin which changes vertices
    float z = (float)gui_image->layer * (1.0 / GUI_ELEMENT_LAYER_LAST);
    struct rectangle *rect = &gui_image->texture_rectangle;

    float vertices[4][5] = {
        {0.0, 1.0, z, rect->x, rect->y + rect->height},
        {1.0, 1.0, z, rect->x + rect->width, rect->y + rect->height},
        {1.0, 0.0, z, rect->x + rect->width, rect->y},
        {0.0, 0.0, z, rect->x, rect->y},
    };

    // Adjust vertices for horizontal origin
    switch (gui_image->origin) {
    case GUI_ELEMENT_ORIGIN_LEFT_TOP:
    case GUI_ELEMENT_ORIGIN_LEFT_CENTER:
    case GUI_ELEMENT_ORIGIN_LEFT_BOTTOM:
        break;

    case GUI_ELEMENT_ORIGIN_CENTER_TOP:
    case GUI_ELEMENT_ORIGIN_CENTER_CENTER:
    case GUI_ELEMENT_ORIGIN_CENTER_BOTTOM:
        for (int i = 0; i < 4; i++) {
            vertices[i][0] -= 0.5;
        }

        break;

    case GUI_ELEMENT_ORIGIN_RIGHT_TOP:
    case GUI_ELEMENT_ORIGIN_RIGHT_CENTER:
    case GUI_ELEMENT_ORIGIN_RIGHT_BOTTOM:
        for (int i = 0; i < 4; i++) {
            vertices[i][0] -= 1;
        }

        break;
    }

    // Adjust vertices for vertical origin
    switch (gui_image->origin) {
    case GUI_ELEMENT_ORIGIN_LEFT_TOP:
    case GUI_ELEMENT_ORIGIN_CENTER_TOP:
    case GUI_ELEMENT_ORIGIN_RIGHT_TOP:
        break;

    case GUI_ELEMENT_ORIGIN_LEFT_CENTER:
    case GUI_ELEMENT_ORIGIN_CENTER_CENTER:
    case GUI_ELEMENT_ORIGIN_RIGHT_CENTER:
        for (int i = 0; i < 4; i++) {
            vertices[i][1] -= 0.5;
        }

        break;

    case GUI_ELEMENT_ORIGIN_LEFT_BOTTOM:
    case GUI_ELEMENT_ORIGIN_CENTER_BOTTOM:
    case GUI_ELEMENT_ORIGIN_RIGHT_BOTTOM:
        for (int i = 0; i < 4; i++) {
            vertices[i][1] -= 1;
        }

        break;
    }

    for (int i = 0; i < 4; i++) {
        vertices[i][0] *=
            gui_image->texture.size.x * rect->width * gui_image->scale.x;
        vertices[i][1] *=
            gui_image->texture.size.y * rect->height * gui_image->scale.y;

        vertices[i][0] += gui_image->position.x;
        vertices[i][1] += gui_image->position.y;
    }

    bo_upload(&gui_image->vbo, 4 * 5 * sizeof(float), vertices,
              BO_USAGE_STATIC_DRAW);
}

// paramter for texture filter
void gui_image_init(struct gui_image *gui_image, char *image_path,
                    struct vec2d position, struct vec2d scale,
                    enum gui_element_origin origin,
                    enum gui_element_layer layer) {
    gui_image->visible = true;
    gui_image->texture_rectangle = (struct rectangle){0, 0, 1, 1};
    gui_image->position = position;
    gui_image->scale = scale;
    gui_image->origin = origin;
    gui_image->layer = layer;

    texture_init(&gui_image->texture, TEXTURE_FILTER_NEAREST,
                 TEXTURE_WRAP_BORDER);
    texture_load(&gui_image->texture, image_path);

    vao_init(&gui_image->vao);
    bo_init(&gui_image->vbo, BO_TYPE_VERTEX);
    bo_init(&gui_image->ibo, BO_TYPE_INDEX);

    vao_bind(&gui_image->vao);
    bo_bind(&gui_image->ibo);

    gui_image_update(gui_image);

    unsigned int indices[6] = {0, 1, 2, 0, 2, 3};
    bo_upload(&gui_image->ibo, 6 * sizeof(unsigned int), indices,
              BO_USAGE_STATIC_DRAW);

    vao_attrib(&gui_image->vao, 0, 3, VAO_TYPE_FLOAT, false, 5 * sizeof(float),
               0);
    vao_attrib(&gui_image->vao, 1, 2, VAO_TYPE_FLOAT, false, 5 * sizeof(float),
               (void *)(3 * sizeof(float)));

    gui_image_update(gui_image);
}

void gui_image_draw(struct gui_image *gui_image) {
    if (!gui_image->visible) {
        return;
    }

    texture_bind(&gui_image->texture);
    vao_bind(&gui_image->vao);
    bo_bind(&gui_image->ibo);
    bo_bind(&gui_image->vbo);

    renderer_draw_elements(DRAW_MODE_TRIANGLES, GUI_IMAGE_INDEX_COUNT,
                           INDEX_TYPE_UNSIGNED_INT);
}

void gui_image_set_visible(struct gui_image *gui_image, bool visible) {
    gui_image->visible = visible;
    gui_image_update(gui_image);
}

void gui_image_destroy(struct gui_image *gui_image) {
    vao_destroy(&gui_image->vao);
    bo_destroy(&gui_image->vbo);
    bo_destroy(&gui_image->ibo);
}

void gui_image_set_scale(struct gui_image *gui_image, struct vec2d scale) {
    gui_image->scale = scale;
    gui_image_update(gui_image);
}

void gui_image_set_position(struct gui_image *gui_image,
                            struct vec2d position) {
    gui_image->position = position;
    gui_image_update(gui_image);
}

void gui_image_set_texture_rectangle(struct gui_image *gui_image,
                                     struct rectangle texture_rectangle) {
    gui_image->texture_rectangle = texture_rectangle;
    gui_image_update(gui_image);
}
