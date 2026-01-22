#include "shader_program.h"

#include <stdio.h>
#include <stdlib.h>
#include "../util/gl.h"
#include "../util/io.h"

void shader_program_init(struct shader_program *program,
                         struct shader *vertex_shader,
                         struct shader *fragment_shader) {
    program->gl_id = GL_CALL_R(glCreateProgram(), GLuint);
    GL_CALL(glAttachShader(program->gl_id, vertex_shader->gl_id));
    GL_CALL(glAttachShader(program->gl_id, fragment_shader->gl_id));

    shader_program_link(program);

    GL_CALL(glDeleteShader(vertex_shader->gl_id));
    GL_CALL(glDeleteShader(fragment_shader->gl_id));
}

void shader_program_from_files(struct shader_program *program,
                               char *vertex_path, char *fragment_path) {
    char *vertex_source = read_file(vertex_path);
    char *fragment_source = read_file(fragment_path);

    if (!vertex_source) {
        fprintf(stderr, "shader_program_from_files: vertex source is null\n");

        return;
    }

    if (!fragment_source) {
        fprintf(stderr, "shader_program_from_files: fragment source is null\n");

        return;
    }

    struct shader vertex_shader;
    shader_init(&vertex_shader, SHADER_TYPE_VERTEX);
    shader_source(&vertex_shader, vertex_source);
    shader_compile(&vertex_shader);

    struct shader fragment_shader;
    shader_init(&fragment_shader, SHADER_TYPE_FRAGMENT);
    shader_source(&fragment_shader, fragment_source);
    shader_compile(&fragment_shader);

    shader_program_init(program, &vertex_shader, &fragment_shader);

    free(vertex_source); // ADDEDD
    free(fragment_source);
}

void shader_program_bind_attribute(struct shader_program *program, int index,
                                   char *name) {

    GL_CALL(glBindAttribLocation(program->gl_id, index, name));
}

void shader_program_link(struct shader_program *program) {
    GL_CALL(glLinkProgram(program->gl_id));
}

void shader_program_use(struct shader_program *program) {
    GL_CALL(glUseProgram(program->gl_id));
}

void shader_program_destroy(struct shader_program *program) {
    GL_CALL(glDeleteProgram(program->gl_id));
}
