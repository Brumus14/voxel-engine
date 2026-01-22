#include "vec2_convert.h"

void vec2d_to_vec2i_floor(struct vec2d v, struct vec2i *dest) {
    dest->x = floor(v.x);
    dest->y = floor(v.y);
}

// Should these take pointers instead
struct vec2i vec2i_from_vec2d_floor(struct vec2d v) {
    return (struct vec2i){
        floor(v.x),
        floor(v.y),
    };
}

void vec2f_to_vec2i_floor(struct vec2f v, struct vec2i *dest) {
    dest->x = floor(v.x);
    dest->y = floor(v.y);
}

struct vec2i vec2i_from_vec2f_floor(struct vec2f v) {
    return (struct vec2i){
        floor(v.x),
        floor(v.y),
    };
}
