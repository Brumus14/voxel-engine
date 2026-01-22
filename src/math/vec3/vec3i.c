#include "vec3i.h"

void vec3i_init(struct vec3i *v, int x, int y, int z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3i_add_to(struct vec3i v1, struct vec3i v2, struct vec3i *dest) {
    dest->x = v1.x + v2.x;
    dest->y = v1.y + v2.y;
    dest->z = v1.z + v2.z;
}

struct vec3i vec3i_add(struct vec3i v1, struct vec3i v2) {
    struct vec3i result;

    vec3i_add_to(v1, v2, &result);

    return result;
}

struct vec3i vec3i_sub(struct vec3i v1, struct vec3i v2) {
    struct vec3i result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;

    return result;
}

void vec3i_scalar_multiply_to(struct vec3i v, int scalar, struct vec3i *dest) {
    dest->x = v.x * scalar;
    dest->y = v.y * scalar;
    dest->z = v.z * scalar;
}

struct vec3i vec3i_scalar_multiply(struct vec3i v, int scalar) {
    struct vec3i result;

    vec3i_scalar_multiply_to(v, scalar, &result);

    return result;
}

bool vec3i_equal(struct vec3i v1, struct vec3i v2) {
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
        return true;
    } else {
        return false;
    }
}
