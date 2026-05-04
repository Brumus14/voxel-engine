#ifndef BLOCK_H
#define BLOCK_H

#include <stdbool.h>
#include "../util/direction.h"

enum block_type {
    BLOCK_TYPE_EMPTY = 0,
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_DIRT,
    BLOCK_TYPE_STONE,
    BLOCK_TYPE_COAL,
    BLOCK_TYPE_LOG,
    BLOCK_TYPE_DIAMOND,
    BLOCK_TYPE_LEAF,
};

enum block_face {
    BLOCK_FACE_LEFT = 0,
    BLOCK_FACE_RIGHT,
    BLOCK_FACE_BOTTOM,
    BLOCK_FACE_TOP,
    BLOCK_FACE_BACK,
    BLOCK_FACE_FRONT,
};

struct block_data {
    unsigned int face_tile_indices[6];
    bool solid; // Rename this
    bool collidable;
};

static const struct block_data block_data[] = {
    [BLOCK_TYPE_EMPTY] = {{-1, -1, -1, -1, -1, -1}, false, false},
    [BLOCK_TYPE_GRASS] = {{1, 1, 2, 0, 1, 1}, true, true},
    [BLOCK_TYPE_DIRT] = {{3, 3, 2, 2, 3, 3}, true, true},
    [BLOCK_TYPE_STONE] = {{4, 4, 4, 4, 4, 4}, true, true},
    [BLOCK_TYPE_COAL] = {{5, 5, 5, 5, 5, 5}, true, true},
    [BLOCK_TYPE_LOG] = {{6, 6, 7, 7, 6, 6}, true, true},
    [BLOCK_TYPE_DIAMOND] = {{8, 8, 8, 8, 8, 8}, true, true},
    [BLOCK_TYPE_LEAF] = {{9, 9, 9, 9, 9, 9}, false, true},
};

unsigned int block_get_face_tile_index(enum block_type type,
                                       enum direction face);
bool block_is_solid(enum block_type type);
bool block_is_collidable(enum block_type type);

struct block {
    enum block_type type;
    // orientation
    // light level
};

#endif
