#ifndef RECTANGLE_H
#define RECTANGLE_H

struct rectangle {
    float x;
    float y;
    float width;
    float height;
};

void rectangle_init(struct rectangle *rectangle, float x, float y, float width,
                    float height);

#endif
