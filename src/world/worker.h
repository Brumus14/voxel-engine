#ifndef WORKER_H
#define WORKER_H

#include "chunk.h"
#include "world.h"

struct worker_generate_chunk_terrain_args {
    struct chunk *chunk;
    int seed;
};

struct worker_generate_chunk_mesh_args {
    struct chunk *chunk;
    struct world *world;
    struct chunk *neighbors[6];
};

void *worker_generate_chunk_terrain(void *arg);
void *worker_generate_chunk_mesh(void *arg);

#endif
