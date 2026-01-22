#include "vec2d.h"

#include "math.h"

void vec2d_init(struct vec2d *v, double x, double y) {
    v->x = x;
    v->y = y;
}

void vec2d_normalise(struct vec2d *v) {
    double magnitude = sqrt(pow(v->x, 2) + pow(v->y, 2));
    if (magnitude == 0) {
        vec2d_init(v, 0.0, 0.0);
    } else {
        v->x /= magnitude;
        v->y /= magnitude;
    }
}

struct vec2d vec2d_normalised(struct vec2d v) {
    struct vec2d result;
    vec2d_init(&result, 0.0, 0.0);

    double magnitude = sqrt(pow(v.x, 2) + pow(v.y, 2));

    if (magnitude != 0) {
        result.x = v.x / magnitude;
        result.y = v.y / magnitude;
    }

    return result;
}

void vec2d_to_glm(struct vec2d v, vec2 *destination) {
    (*destination)[0] = v.x;
    (*destination)[1] = v.y;
}

struct vec2d vec2d_add(struct vec2d v1, struct vec2d v2) {
    struct vec2d result;

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;

    return result;
}

struct vec2d vec2d_sub(struct vec2d v1, struct vec2d v2) {
    struct vec2d result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;

    return result;
}

struct vec2d vec2d_scalar_multiply(struct vec2d v, double scalar) {
    struct vec2d result;

    result.x = v.x * scalar;
    result.y = v.y * scalar;

    return result;
}
