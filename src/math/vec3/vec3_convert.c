#include "vec3_convert.h"

void vec3d_to_vec3i_floor(struct vec3d v, struct vec3i *dest) {
    dest->x = floor(v.x);
    dest->y = floor(v.y);
    dest->z = floor(v.z);
}

struct vec3i vec3i_from_vec3d_floor(struct vec3d v) {
    return (struct vec3i){
        floor(v.x),
        floor(v.y),
        floor(v.z),
    };
}

void vec3f_to_vec3i_floor(struct vec3f v, struct vec3i *dest) {
    dest->x = floor(v.x);
    dest->y = floor(v.y);
    dest->z = floor(v.z);
}

struct vec3i vec3i_from_vec3f_floor(struct vec3f v) {
    return (struct vec3i){
        floor(v.x),
        floor(v.y),
        floor(v.z),
    };
}
