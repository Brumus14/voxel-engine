#ifndef VEC3D_H
#define VEC3D_H

#include "cglm/cglm.h"
#include "vec3i.h"

#define EPSILON 1e-6

struct vec3d {
    double x;
    double y;
    double z;
};

#define VEC3D_ZERO (struct vec3d){0.0, 0.0, 0.0}

void vec3d_init(struct vec3d *v, double x, double y, double z);
double vec3d_magnitude(struct vec3d v);
void vec3d_normalise(struct vec3d *v);
struct vec3d vec3d_normalised(struct vec3d v);
void vec3d_to_glm(struct vec3d v, vec3 *destination);
void vec3d_add_to(struct vec3d v1, struct vec3d v2, struct vec3d *dest);
struct vec3d vec3d_add(struct vec3d v1, struct vec3d v2);
void vec3d_sub_to(struct vec3d v1, struct vec3d v2, struct vec3d *dest);
struct vec3d vec3d_sub(struct vec3d v1, struct vec3d v2);
void vec3d_scalar_multiply_to(struct vec3d v, double scalar,
                              struct vec3d *dest);
struct vec3d vec3d_scalar_multiply(struct vec3d v, double scalar);
void vec3d_cross_product_to(struct vec3d v1, struct vec3d v2,
                            struct vec3d *dest);
struct vec3d vec3d_cross_product(struct vec3d v1, struct vec3d v2);
void vec3d_dot_product_to(struct vec3d v1, struct vec3d v2, struct vec3d *dest);
struct vec3d vec3d_dot_product(struct vec3d v1, struct vec3d v2);
bool vec3d_equal(struct vec3d v1, struct vec3d v2);

#endif
