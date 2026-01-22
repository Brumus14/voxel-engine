#ifndef VEC2_CONVERT_H
#define VEC2_CONVERT_H

#include "vec2i.h"
#include "vec2f.h"
#include "vec2d.h"

void vec2d_to_vec2i_floor(struct vec2d v, struct vec2i *dest);
struct vec2i vec2i_from_vec2d_floor(struct vec2d v);
void vec2f_to_vec2i_floor(struct vec2f v, struct vec2i *dest);
struct vec2i vec2i_from_vec2f_floor(struct vec2f v);

#endif
