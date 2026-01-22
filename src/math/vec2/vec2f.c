#include "vec2f.h"

#include "math.h"

void vec2f_init(struct vec2f *v, float x, float y) {
    v->x = x;
    v->y = y;
}

void vec2f_normalise(struct vec2f *v) {
    float magnitude = sqrt(pow(v->x, 2) + pow(v->y, 2));
    if (magnitude == 0) {
        vec2f_init(v, 0.0, 0.0);
    } else {
        v->x /= magnitude;
        v->y /= magnitude;
    }
}

struct vec2f vec2f_normalised(struct vec2f v) {
    struct vec2f result;
    vec2f_init(&result, 0.0, 0.0);

    float magnitude = sqrt(pow(v.x, 2) + pow(v.y, 2));

    if (magnitude != 0) {
        result.x = v.x / magnitude;
        result.y = v.y / magnitude;
    }

    return result;
}

void vec2f_to_glm(struct vec2f v, vec2 *destination) {
    (*destination)[0] = v.x;
    (*destination)[1] = v.y;
}

struct vec2f vec2f_add(struct vec2f v1, struct vec2f v2) {
    struct vec2f result;

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;

    return result;
}

struct vec2f vec2f_sub(struct vec2f v1, struct vec2f v2) {
    struct vec2f result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;

    return result;
}

struct vec2f vec2f_scalar_multiply(struct vec2f v, float scalar) {
    struct vec2f result;

    result.x = v.x * scalar;
    result.y = v.y * scalar;

    return result;
}
