#include "cuboid.h"

void cuboid_init(struct cuboid *cuboid, float x, float y, float z, float width,
                 float height, float depth) {
    cuboid->x = x;
    cuboid->y = y;
    cuboid->z = z;
    cuboid->width = width;
    cuboid->height = height;
    cuboid->depth = depth;
}
