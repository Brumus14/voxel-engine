#ifndef VEC3F_H
#define VEC3F_H

#include "cglm/types.h"
#include <stdbool.h>

#define EPSILON 1e-6

struct vec3f {
    float x;
    float y;
    float z;
};

#define VEC3F_ZERO (struct vec3f){0.0, 0.0, 0.0}

void vec3f_init(struct vec3f *v, float x, float y, float z);
void vec3f_normalise(struct vec3f *vec);
struct vec3f vec3f_normalised(struct vec3f vec);
void vec3f_to_glm(struct vec3f v, vec3 *destination);
void vec3f_add_to(struct vec3f v1, struct vec3f v2, struct vec3f *dest);
struct vec3f vec3f_add(struct vec3f v1, struct vec3f v2);
struct vec3f vec3f_sub(struct vec3f v1, struct vec3f v2);
void vec3f_scalar_multiply_to(struct vec3f v, float scalar, struct vec3f *dest);
struct vec3f vec3f_scalar_multiply(struct vec3f v, float scalar);
struct vec3f vec3f_cross_product(struct vec3f v1, struct vec3f v2);
bool vec3f_equal(struct vec3f v1, struct vec3f v2);

#endif
