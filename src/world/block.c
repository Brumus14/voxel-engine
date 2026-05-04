#include "block.h"

unsigned int block_get_face_tile_index(enum block_type type,
                                       enum direction face) {
    return block_data[type].face_tile_indices[face];
}

bool block_is_solid(enum block_type type) {
    return block_data[type].solid;
}

bool block_is_collidable(enum block_type type) {
    return block_data[type].collidable;
}
