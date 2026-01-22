#ifndef VEC2I_H
#define VEC2I_H

#include "cglm/cglm.h"

struct vec2i {
    int x;
    int y;
};

#define VEC2I_ZERO (struct vec2i){0, 0}

void vec2i_init(struct vec2i *v, int x, int y);
void vec2i_to_glm(struct vec2i v, vec2 *destination);
struct vec2i vec2i_add(struct vec2i v1, struct vec2i v2);
struct vec2i vec2i_sub(struct vec2i v1, struct vec2i v2);
struct vec2i vec2i_scalar_multiply(struct vec2i v, int scalar);

#endif
