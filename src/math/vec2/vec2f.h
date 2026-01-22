#ifndef VEC2F_H
#define VEC2F_H

#include "cglm/cglm.h"

struct vec2f {
    float x;
    float y;
};

#define VEC2F_ZERO (struct vec2f){0.0, 0.0}

void vec2f_init(struct vec2f *v, float x, float y);
void vec2f_normalise(struct vec2f *v);
struct vec2f vec2f_normalised(struct vec2f v);
void vec2f_to_glm(struct vec2f v, vec2 *destination);
struct vec2f vec2f_add(struct vec2f v1, struct vec2f v2);
struct vec2f vec2f_sub(struct vec2f v1, struct vec2f v2);
struct vec2f vec2f_scalar_multiply(struct vec2f v, float scalar);

#endif
