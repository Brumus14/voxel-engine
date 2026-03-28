#ifndef WORLD_GENERATION_H
#define WORLD_GENERATION_H

#include "world.h"

_Atomic(enum block_type) *
world_generation_chunk_terrain(struct vec3i chunk_position, int seed);

#endif
