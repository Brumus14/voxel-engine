#include "vec3d.h"

#include "math.h"

void vec3d_init(struct vec3d *v, double x, double y, double z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

double vec3d_magnitude(struct vec3d v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

void vec3d_normalise(struct vec3d *v) {
    double magnitude = vec3d_magnitude(*v);

    if (magnitude > EPSILON) {
        v->x /= magnitude;
        v->y /= magnitude;
        v->z /= magnitude;
    }
}

struct vec3d vec3d_normalised(struct vec3d v) {
    struct vec3d result = VEC3D_ZERO;

    double magnitude = vec3d_magnitude(v);

    if (magnitude > EPSILON) {
        result.x = v.x / magnitude;
        result.y = v.y / magnitude;
        result.z = v.z / magnitude;
    }

    return result;
}

void vec3d_to_glm(struct vec3d v, vec3 *destination) {
    (*destination)[0] = v.x;
    (*destination)[1] = v.y;
    (*destination)[2] = v.z;
}

void vec3d_add_to(struct vec3d v1, struct vec3d v2, struct vec3d *dest) {
    dest->x = v1.x + v2.x;
    dest->y = v1.y + v2.y;
    dest->z = v1.z + v2.z;
}

struct vec3d vec3d_add(struct vec3d v1, struct vec3d v2) {
    struct vec3d result;

    vec3d_add_to(v1, v2, &result);

    return result;
}

void vec3d_sub_to(struct vec3d v1, struct vec3d v2, struct vec3d *dest) {
    dest->x = v1.x - v2.x;
    dest->y = v1.y - v2.y;
    dest->z = v1.z - v2.z;
}

struct vec3d vec3d_sub(struct vec3d v1, struct vec3d v2) {
    struct vec3d result;

    vec3d_sub_to(v1, v2, &result);

    return result;
}

void vec3d_scalar_multiply_to(struct vec3d v, double scalar,
                              struct vec3d *dest) {
    dest->x = v.x * scalar;
    dest->y = v.y * scalar;
    dest->z = v.z * scalar;
}

struct vec3d vec3d_scalar_multiply(struct vec3d v, double scalar) {
    struct vec3d result;

    vec3d_scalar_multiply_to(v, scalar, &result);

    return result;
}

void vec3d_cross_product_to(struct vec3d v1, struct vec3d v2,
                            struct vec3d *dest) {
    dest->x = v1.y * v2.z - v1.z * v2.y;
    dest->y = v1.z * v2.x - v1.x * v2.z;
    dest->z = v1.x * v2.y - v1.y * v2.x;
}

struct vec3d vec3d_cross_product(struct vec3d v1, struct vec3d v2) {
    struct vec3d result;

    vec3d_cross_product_to(v1, v2, &result);

    return result;
}

void vec3d_dot_product_to(struct vec3d v1, struct vec3d v2,
                          struct vec3d *dest) {
    dest->x = v1.x * v2.x;
    dest->y = v1.y * v2.y;
    dest->z = v1.z * v2.z;
}

struct vec3d vec3d_dot_product(struct vec3d v1, struct vec3d v2) {
    struct vec3d result;

    vec3d_dot_product_to(v1, v2, &result);

    return result;
}

bool vec3d_equal(struct vec3d v1, struct vec3d v2) {
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
        return true;
    } else {
        return false;
    }
}

// vec3i vec3d_to_vec3i(struct vec3d v) {
//     return (vec3i){round(vec.x), round(vec.y), round(vec.z)};
// }
