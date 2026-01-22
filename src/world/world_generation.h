#ifndef WORLD_GENERATION_H
#define WORLD_GENERATION_H

#include "world.h"

enum block_type *world_generation_chunk_terrain(struct vec3i chunk_position,
                                                float seed);

#endif
