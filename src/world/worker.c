#include "worker.h"
#include "block.h"
#include "chunk.h"
#include "world_generation.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO: Check if the struct chunk has been unloaded if so then cancel
// Doesn't free arg
void *worker_generate_chunk_terrain(void *arg) {
    struct worker_generate_chunk_terrain_args *args = arg;
    struct chunk *chunk = args->chunk;
    float seed = args->seed;

    atomic_fetch_add(&chunk->in_use, 1);
    atomic_store(&chunk->state, CHUNK_STATE_GENERATING_TERRAIN);

    enum block_type *terrain =
        world_generation_chunk_terrain(chunk->position, seed);

    pthread_mutex_lock(&chunk->lock);
    memcpy(chunk->blocks, terrain, sizeof(chunk->blocks));

    printf("generated terrain for %d,%d,%d\n", chunk->position.x,
           chunk->position.y, chunk->position.z);
    pthread_mutex_unlock(&chunk->lock);

    free(terrain);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_MESH);
    atomic_fetch_sub(&chunk->in_use, 1);

    return NULL;
}

void *worker_generate_chunk_mesh(void *arg) {
    struct worker_generate_chunk_mesh_args *args = arg;
    struct chunk *chunk = args->chunk;
    struct world *world = args->world;
    struct chunk **neighbors = args->neighbors;

    atomic_fetch_add(&chunk->in_use, 1);
    atomic_store(&chunk->state, CHUNK_STATE_GENERATING_MESH);

    chunk_generate_mesh(chunk, neighbors);

    printf("generated mesh for %d,%d,%d\n", chunk->position.x,
           chunk->position.y, chunk->position.z);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_BUFFERS);
    atomic_fetch_sub(&chunk->in_use, 1);

    return NULL;
}

// Rename to init chunk?
// Remove and just queue terrain, then queue mesh when terrain done?
void *worker_generate_chunk(void *arg) {
    struct worker_generate_chunk_args *args = arg;
    struct chunk *chunk = args->chunk;
    float seed = args->seed;
    struct thread_pool *workers = args->workers;

    atomic_fetch_add(&chunk->in_use, 1);

    struct worker_generate_chunk_terrain_args terrain_args = {chunk, seed};

    worker_generate_chunk_terrain(&terrain_args);

    atomic_fetch_sub(&chunk->in_use, 1);

    free(arg);

    return NULL;
}
