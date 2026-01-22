#include "collision.h"

bool collision_aabb_2d(struct rectangle r1, struct rectangle r2) {
    return (r1.x <= r2.x + r2.width && r1.x + r1.width >= r2.x &&
            r1.y <= r2.y + r2.height && r1.y + r1.height >= r2.y);
}

bool collision_aabb_3d(struct cuboid c1, struct cuboid c2) {
    return (c1.x <= c2.x + c2.width && c1.x + c1.width >= c2.x &&
            c1.y <= c2.y + c2.height && c1.y + c1.height >= c2.y &&
            c1.z <= c2.z + c2.depth && c1.z + c1.depth >= c2.z);
}
