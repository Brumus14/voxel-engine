#ifndef VEC2D_H
#define VEC2D_H

#include "cglm/cglm.h"

struct vec2d {
    double x;
    double y;
};

#define VEC2D_ZERO (struct vec2d){0.0, 0.0}

void vec2d_init(struct vec2d *v, double x, double y);
void vec2d_normalise(struct vec2d *v);
struct vec2d vec2d_normalised(struct vec2d v);
void vec2d_to_glm(struct vec2d v, vec2 *destination);
struct vec2d vec2d_add(struct vec2d v1, struct vec2d v2);
struct vec2d vec2d_sub(struct vec2d v1, struct vec2d v2);
struct vec2d vec2d_scalar_multiply(struct vec2d v, double scalar);

#endif
