#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include "../math/rectangle.h"
#include "../math/cuboid.h"

bool collision_aabb_2d(struct rectangle r1, struct rectangle r2);
bool collision_aabb_3d(struct cuboid c1, struct cuboid c2);

#endif
