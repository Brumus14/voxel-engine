#ifndef VEC3I_H
#define VEC3I_H

#include "cglm/cglm.h"

struct vec3i {
    int x;
    int y;
    int z;
};

#define VEC3I_ZERO (struct vec3i){0, 0, 0}

void vec3i_init(struct vec3i *v, int x, int y, int z);
void vec3i_normalise(struct vec3i *v);
struct vec3i vec3i_normalised(struct vec3i v);
void vec3i_to_glm(struct vec3i v, vec3 *destination);
void vec3i_add_to(struct vec3i v1, struct vec3i v2, struct vec3i *dest);
struct vec3i vec3i_add(struct vec3i v1, struct vec3i v2);
struct vec3i vec3i_sub(struct vec3i v1, struct vec3i v2);
void vec3i_scalar_multiply_to(struct vec3i v, int scalar, struct vec3i *dest);
struct vec3i vec3i_scalar_multiply(struct vec3i v, int scalar);
struct vec3i vec3i_cross_product(struct vec3i v1, struct vec3i v2);
bool vec3i_equal(struct vec3i v1, struct vec3i v2);

#endif
