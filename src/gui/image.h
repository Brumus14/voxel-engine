#ifndef GUI_IMAGE_H
#define GUI_IMAGE_H

#define GUI_IMAGE_INDEX_COUNT 6

#include "../graphics/texture.h"
#include "../graphics/vao.h"
#include "../graphics/bo.h"
#include "../math/rectangle.h"
#include "element.h"

struct gui_image {
    bool visible;
    struct vec2d position;
    struct vec2d scale;
    struct texture texture;
    struct rectangle texture_rectangle;
    enum gui_element_origin origin;
    enum gui_element_layer layer;
    struct vao vao;
    struct bo vbo;
    struct bo ibo;
};

void gui_image_init(struct gui_image *gui_image, char *image_path,
                    struct vec2d position, struct vec2d scale,
                    enum gui_element_origin origin,
                    enum gui_element_layer layer);
void gui_image_destroy(struct gui_image *gui_image);
void gui_image_draw(struct gui_image *gui_image);

void gui_image_set_visible(struct gui_image *gui_image, bool visible);
void gui_image_set_position(struct gui_image *gui_image, struct vec2d position);
void gui_image_set_scale(struct gui_image *gui_image, struct vec2d scale);
void gui_image_set_texture_rectangle(struct gui_image *gui_image,
                                     struct rectangle texture_rectangle);
void gui_image_set_origin(struct gui_image *gui_image,
                          enum gui_element_origin origin);
void gui_image_set_layer(struct gui_image *gui_image,
                         enum gui_element_layer layer);

#endif
