#ifndef VBO_H
#define VBO_H

#include "glad/glad.h"

enum bo_type { BO_TYPE_VERTEX, BO_TYPE_INDEX };

struct bo {
    GLuint gl_id;
    enum bo_type type;
};

enum bo_usage {
    BO_USAGE_STREAM_DRAW,
    BO_USAGE_STREAM_READ,
    BO_USAGE_STREAM_COPY,
    BO_USAGE_STATIC_DRAW,
    BO_USAGE_STATIC_READ,
    BO_USAGE_STATIC_COPY,
    BO_USAGE_DYNAMIC_DRAW,
    BO_USAGE_DYNAMIC_READ,
    BO_USAGE_DYNAMIC_COPY
};

GLenum to_gl_usage(enum bo_usage usage);
GLenum to_gl_type(enum bo_type type);
void bo_init(struct bo *bo, enum bo_type type);
void bo_bind(struct bo *bo);
void bo_upload(struct bo *bo, int data_size, void *data, enum bo_usage usage);
void bo_destroy(struct bo *bo);
int bo_get_size(struct bo *bo);

#endif
