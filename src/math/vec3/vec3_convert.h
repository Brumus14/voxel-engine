#ifndef VEC3_CONVERT_H
#define VEC3_CONVERT_H

#include "vec3i.h"
#include "vec3f.h"
#include "vec3d.h"

void vec3d_to_vec3i_floor(struct vec3d v, struct vec3i *dest);
struct vec3i vec3i_from_vec3d_floor(struct vec3d v);
void vec3f_to_vec3i_floor(struct vec3f v, struct vec3i *dest);
struct vec3i vec3i_from_vec3f_floor(struct vec3f v);

#endif
