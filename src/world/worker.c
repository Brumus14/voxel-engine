#include "worker.h"
#include "block.h"
#include "chunk.h"
#include "world_generation.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../util/logging.h"

// TODO: Check if the struct chunk has been unloaded if so then cancel
// Doesn't free arg
void *worker_generate_chunk_terrain(void *arg) {
    struct worker_generate_chunk_terrain_args *args = arg;
    struct chunk *chunk = args->chunk;
    float seed = args->seed;

    if (atomic_load(&chunk->state) == CHUNK_STATE_UNLOADED) {
        free(arg);
        atomic_fetch_sub(&chunk->ref_count, 1);
        return NULL;
    }

    atomic_store(&chunk->state, CHUNK_STATE_GENERATING_TERRAIN);

    enum block_type *new_blocks =
        world_generation_chunk_terrain(chunk->position, seed);
    enum block_type *old_blocks = chunk->blocks;

    pthread_mutex_lock(&chunk->lock);
    chunk->blocks = new_blocks;

    if (WORLD_LOGGING) {
        printf("generated terrain for %d,%d,%d\n", chunk->position.x,
               chunk->position.y, chunk->position.z);
    }
    pthread_mutex_unlock(&chunk->lock);

    free(old_blocks);

    free(arg);
    atomic_fetch_sub(&chunk->ref_count, 1);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_MESH);

    return NULL;
}

void *worker_generate_chunk_mesh(void *arg) {
    struct worker_generate_chunk_mesh_args *args = arg;
    struct chunk *chunk = args->chunk;
    struct world *world = args->world;
    struct chunk **neighbors = args->neighbors;

    if (atomic_load(&chunk->state) == CHUNK_STATE_UNLOADED) {
        free(arg);
        atomic_fetch_sub(&chunk->ref_count, 1);
        return NULL;
    }

    atomic_store(&chunk->state, CHUNK_STATE_GENERATING_MESH);

    pthread_mutex_lock(&chunk->lock);
    chunk_generate_mesh(chunk, neighbors);

    WORLD_LOG(printf("generated mesh for %d,%d,%d\n", chunk->position.x,
                     chunk->position.y, chunk->position.z))
    // if (WORLD_LOGGING) {
    //     printf("generated mesh for %d,%d,%d\n", chunk->position.x,
    //            chunk->position.y, chunk->position.z);
    // }
    pthread_mutex_unlock(&chunk->lock);

    free(arg);
    atomic_fetch_sub(&chunk->ref_count, 1);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_BUFFERS);

    return NULL;
}
