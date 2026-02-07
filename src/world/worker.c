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

    if (atomic_load(&chunk->unloaded)) {
        free(arg);
        atomic_fetch_sub(&chunk->ref_count, 1);
        return NULL;
    }

    atomic_store(&chunk->blocks_state, CHUNK_BLOCKS_STATE_GENERATING);

    enum block_type *new_blocks =
        world_generation_chunk_terrain(chunk->position, seed);
    enum block_type *old_blocks = chunk->blocks;

    // Better locking
    pthread_mutex_lock(&chunk->lock);
    chunk->blocks = new_blocks;

    WORLD_LOG(printf("generated terrain for %d,%d,%d\n", chunk->position.x,
                     chunk->position.y, chunk->position.z));
    pthread_mutex_unlock(&chunk->lock);

    free(old_blocks);

    atomic_store(&chunk->blocks_state, CHUNK_BLOCKS_STATE_GENERATED);

    free(arg);
    atomic_fetch_sub(&chunk->ref_count, 1);

    return NULL;
}

void *worker_generate_chunk_mesh(void *arg) {
    struct worker_generate_chunk_mesh_args *args = arg;
    struct chunk *chunk = args->chunk;
    struct world *world = args->world;
    struct chunk **neighbors = args->neighbors;

    if (atomic_load(&chunk->unloaded)) {
        for (int i = 0; i < 6; i++) {
            atomic_fetch_sub(&neighbors[i]->ref_count, 1);
        }

        free(arg);
        atomic_fetch_sub(&chunk->ref_count, 1);
        return NULL;
    }

    atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_GENERATING);

    pthread_mutex_lock(&chunk->lock);
    chunk_generate_mesh(chunk, neighbors);

    WORLD_LOG(printf("generated mesh for %d,%d,%d\n", chunk->position.x,
                     chunk->position.y, chunk->position.z))
    pthread_mutex_unlock(&chunk->lock);

    for (int i = 0; i < 6; i++) {
        atomic_fetch_sub(&neighbors[i]->ref_count, 1);
    }

    atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_GENERATED);
    atomic_store(&chunk->buffers_stale, true);

    free(arg);
    atomic_fetch_sub(&chunk->ref_count, 1);

    return NULL;
}
