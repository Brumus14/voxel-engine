#include "vec2i.h"

#include "math.h"

void vec2i_init(struct vec2i *v, int x, int y) {
    v->x = x;
    v->y = y;
}

void vec2i_to_glm(struct vec2i v, vec2 *destination) {
    (*destination)[0] = v.x;
    (*destination)[1] = v.y;
}

struct vec2i vec2i_add(struct vec2i v1, struct vec2i v2) {
    struct vec2i result;

    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;

    return result;
}

struct vec2i vec2i_sub(struct vec2i v1, struct vec2i v2) {
    struct vec2i result;

    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;

    return result;
}

struct vec2i vec2i_scalar_multiply(struct vec2i v, int scalar) {
    struct vec2i result;

    result.x = v.x * scalar;
    result.y = v.y * scalar;

    return result;
}
