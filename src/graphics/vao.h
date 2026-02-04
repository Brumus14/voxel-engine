#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <stdio.h>
#include <stdbool.h>

struct vao {
    GLuint gl_id;
};

enum array_type {
    VAO_TYPE_FLOAT = 0,
    VAO_TYPE_INT,
    VAO_TYPE_UINT,
};

void vao_init(struct vao *vao);
void vao_destroy(struct vao *vao);
void vao_bind(struct vao *vao);
void vao_attrib(struct vao *vao, int index, int size, enum array_type type,
                bool normalised, size_t stride, void *pointer);

#endif
