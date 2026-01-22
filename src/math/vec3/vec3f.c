#include "vec3f.h"

#include "math.h"

void vec3f_init(struct vec3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3f_normalise(struct vec3f *v) {
    float magnitude = sqrt(pow(v->x, 2) + pow(v->y, 2) + pow(v->z, 2));

    if (magnitude > EPSILON) {
        v->x /= magnitude;
        v->y /= magnitude;
        v->z /= magnitude;
    }
}

struct vec3f vec3f_normalised(struct vec3f v) {
    struct vec3f result = VEC3F_ZERO;

    float magnitude = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));

    if (magnitude > EPSILON) {
        result.x = v.x / magnitude;
        result.y = v.y / magnitude;
        result.z = v.z / magnitude;
    }

    return result;
}

void vec3f_to_glm(struct vec3f v, vec3 *destination) {
    (*destination)[0] = v.x;
    (*destination)[1] = v.y;
    (*destination)[2] = v.z;
}

void vec3f_add_to(struct vec3f v1, struct vec3f v2, struct vec3f *dest) {
    dest->x = v1.x + v2.x;
    dest->y = v1.y + v2.y;
    dest->z = v1.z + v2.z;
}

struct vec3f vec3f_add(struct vec3f v1, struct vec3f v2) {
    struct vec3f result;

    vec3f_add_to(v1, v2, &result);

    return result;
}

struct vec3f vec3f_sub(struct vec3f v1, struct vec3f v2) {
    struct vec3f result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
}

void vec3f_scalar_multiply_to(struct vec3f vec, float scalar,
                              struct vec3f *dest) {
    dest->x = vec.x * scalar;
    dest->y = vec.y * scalar;
    dest->z = vec.z * scalar;
}

struct vec3f vec3f_scalar_multiply(struct vec3f vec, float scalar) {
    struct vec3f result;

    vec3f_scalar_multiply_to(vec, scalar, &result);

    return result;
}

struct vec3f vec3f_cross_product(struct vec3f v1, struct vec3f v2) {
    struct vec3f result;

    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}

bool vec3f_equal(struct vec3f v1, struct vec3f v2) {
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
        return true;
    } else {
        return false;
    }
}
