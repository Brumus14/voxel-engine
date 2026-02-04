#ifndef UTIL_GL_H
#define UTIL_GL_H

#include <stdio.h>
#include "glad/glad.h"

#define GL_CALL(func) \
    func;             \
    check_gl_error(#func);

#define GL_CALL_R(func, type)  \
    ({                         \
        type result = func;    \
        check_gl_error(#func); \
        result;                \
    })

static inline void check_gl_error(char *prefix) {
    GLenum error = glGetError();

    if (error == 0) {
        return;
    }

    fprintf(stderr, "%s: ", prefix);

    switch (error) {
    case GL_INVALID_ENUM:
        fprintf(stderr, "invalid enum (parameter isnt legal)");
        break;
    case GL_INVALID_VALUE:
        fprintf(stderr, "invalid value (state isnt legal for parameters)");
        break;
    case GL_INVALID_OPERATION:
        fprintf(
            stderr,
            "invalid operation (state for command isnt legal for parameters)");
        break;
    case GL_STACK_OVERFLOW:
        fprintf(stderr, "stack overflow");
        break;
    case GL_STACK_UNDERFLOW:
        fprintf(stderr, "stack underflow");
        break;
    case GL_OUT_OF_MEMORY:
        fprintf(stderr, "out of memory (cant allocate enough memory)");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        fprintf(stderr, "invalid framebuffer operation (reading or writing to "
                        "incomplete framebuffer)");
        break;
    }

    fputc('\n', stderr);
}

#endif
