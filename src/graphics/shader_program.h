#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "shader.h"

struct shader_program {
    GLuint gl_id;
    /*int shader_keys[5];*/
    /*int shader_gl_id_values[5];*/
};

void shader_program_init(struct shader_program *program,
                         struct shader *vertex_shader,
                         struct shader *fragment_shader);
void shader_program_from_files(struct shader_program *program,
                               char *vertex_path, char *fragment_path);
void shader_program_bind_attribute(struct shader_program *program, int index,
                                   char *name);
void shader_program_link(struct shader_program *program);
void shader_program_use(struct shader_program *program);
void shader_program_destroy(struct shader_program *program);

#endif
