#ifndef DIRECTION_H
#define DIRECTION_H

#include "../math/vec3.h"

enum direction {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT,
    DIRECTION_BOTTOM,
    DIRECTION_TOP,
    DIRECTION_BACK,
    DIRECTION_FRONT,
};

// clang-format off
static const struct vec3i DIRECTIONS[6] = {
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1},
};
// clang-format on

#endif
