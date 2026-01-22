#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad/glad.h"
#include "../math/vec2.h"

enum texture_filter {
    TEXTURE_FILTER_LINEAR,
    TEXTURE_FILTER_NEAREST,
};

enum texture_wrap {
    TEXTURE_WRAP_BORDER,
    TEXTURE_WRAP_REPEAT,
};

struct texture {
    GLuint gl_id;
    enum texture_filter filter;
    enum texture_wrap wrap;
    struct vec2i size;
};

void texture_init(struct texture *texture, enum texture_filter filter,
                  enum texture_wrap wrap);
void texture_bind(struct texture *texture);
void texture_load(struct texture *texture, char *path);

#endif
