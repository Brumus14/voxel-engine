#ifndef CUBOID_H
#define CUBOID_H

struct cuboid {
    float x;
    float y;
    float z;
    float width;
    float height;
    float depth;
};

void cuboid_init(struct cuboid *cuboid, float x, float y, float z, float width,
                 float height, float depth);

#endif
