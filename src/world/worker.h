#ifndef WORKER_H
#define WORKER_H

#include "chunk.h"
#include "world.h"

#include "../util/thread_pool.h"
#include "../data_structures/hash_map.h"

struct worker_generate_chunk_terrain_args {
    struct chunk *chunk;
    float seed;
};

struct worker_generate_chunk_mesh_args {
    struct chunk *chunk;
    struct world *world;
    // hash_map *chunks;
};

struct worker_generate_chunk_args {
    struct chunk *chunk;
    float seed;
    struct thread_pool *workers;
};

void *worker_generate_chunk_terrain(void *arg);
void *worker_generate_chunk_mesh(void *arg);
void *worker_generate_chunk(void *arg);

#endif
