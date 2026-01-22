#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

struct shader {
    GLuint gl_id;
};

enum shader_type { SHADER_TYPE_VERTEX, SHADER_TYPE_FRAGMENT };

void shader_init(struct shader *shader, enum shader_type type);
void shader_source(struct shader *shader, char *source);
void shader_compile(struct shader *shader);
void shader_delete(struct shader *shader);

#endif
