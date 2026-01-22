#ifndef RENDERER_H
#define RENDERER_H

#include "glad/glad.h"

enum draw_mode { DRAW_MODE_TRIANGLES };

enum polygon_mode {
    POLYGON_MODE_FILL,
    POLYGON_MODE_LINE,
};

enum index_type {
    INDEX_TYPE_UNSIGNED_INT,
    INDEX_TYPE_UNSIGNED_SHORT,
    INDEX_TYPE_UNSIGNED_BYTE,
};

void renderer_init();
void renderer_clear_buffers();
void renderer_clear_depth_buffer();
void renderer_set_clear_colour(float red, float green, float blue, float alpha);
void renderer_set_viewport(int x, int y, int width, int height);
void renderer_draw_elements(enum draw_mode mode, int count,
                            enum index_type type);
void renderer_set_polygon_mode(enum polygon_mode mode);

#endif
