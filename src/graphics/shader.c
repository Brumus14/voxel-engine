#include "shader.h"

#include <stdio.h>
#include "../util/gl.h"

GLenum to_shader_type_gl(enum shader_type type) {
    switch (type) {
    case SHADER_TYPE_VERTEX:
        return GL_VERTEX_SHADER;
    case SHADER_TYPE_FRAGMENT:
        return GL_FRAGMENT_SHADER;
    }
}

void shader_init(struct shader *shader, enum shader_type type) {
    if (!shader) {
        fprintf(stderr, "shader_init: struct shader is null\n");
        return;
    }

    GLenum gl_type = to_shader_type_gl(type);

    shader->gl_id = GL_CALL_R(glCreateShader(gl_type), GLuint);
}

void shader_destroy(struct shader *shader) {
    if (!shader) {
        fprintf(stderr, "shader_delete: struct shader is null\n");
        return;
    }

    GL_CALL(glDeleteShader(shader->gl_id));
}

void shader_source(struct shader *shader, char *source) {
    if (!shader) {
        fprintf(stderr, "shader_source: struct shader is null\n");
        return;
    }

    const GLchar *sources[] = {source};
    GL_CALL(glShaderSource(shader->gl_id, 1, sources, 0));
}

void shader_compile(struct shader *shader) {
    if (!shader) {
        fprintf(stderr, "shader_compile: struct shader is null\n");
        return;
    }

    GL_CALL(glCompileShader(shader->gl_id));

    GLint compile_status;
    GL_CALL(glGetShaderiv(shader->gl_id, GL_COMPILE_STATUS, &compile_status));

    if (compile_status == GL_FALSE) {
        int max_length = 0;
        GL_CALL(glGetShaderiv(shader->gl_id, GL_INFO_LOG_LENGTH, &max_length));

        char error[max_length];
        glGetShaderInfoLog(shader->gl_id, max_length, &max_length,
                           (GLchar *)&error);

        fprintf(stderr, "shader_compile: %s\n", error);
    }
}
