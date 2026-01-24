#include "world.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../math/math_util.h"
#include "../util/stopwatch.h"
#include "chunk.h"
#include "block.h"
#include "worker.h"

unsigned int chunk_hash_function(void *key) {
    struct vec3i *position = key;
    unsigned int hx = position->x * 0x9E3779B185EBCA87;
    unsigned int hy = position->y * 0xC2B2AE3D27D4EB4F;
    unsigned int hz = position->z * 0x165667B19E3779F9;
    return hx ^ hy ^ hz;
}

void world_init(struct world *world) {
    hash_map_init(&world->chunks, CHUNKS_BUCKET_COUNT, sizeof(struct vec3i),
                  sizeof(struct chunk), chunk_hash_function);

    tilemap_init(&world->tilemap, "res/textures/atlas.png",
                 TEXTURE_FILTER_NEAREST, 16, 16, 1, 2);

    world->seed = random_range(0, 100);

    thread_pool_init(&world->workers, WORLD_WORKER_COUNT);
}

void world_load_chunk(struct world *world, struct vec3i position) {
    // Check not already loaded
    bool loaded = hash_map_get(&world->chunks, &position);

    if (loaded) {
        return;
    }

    printf("added ");
    vec3i_print(position);

    struct chunk *new_chunk = malloc(sizeof(struct chunk));
    chunk_init(new_chunk, position, &world->tilemap, false);

    struct chunk *old_value =
        hash_map_put(&world->chunks, &position, new_chunk);
    free(old_value);

    struct worker_generate_chunk_args *args =
        malloc(sizeof(struct worker_generate_chunk_args));
    args->chunk = new_chunk;
    args->seed = world->seed;
    args->workers = &world->workers;

    thread_pool_schedule(&world->workers, worker_generate_chunk, args);
}

// remove chunks from chunks to generate if have moved out of render distance
// before generated
void world_unload_chunk(struct world *world, struct vec3i position) {
    struct chunk *chunk = hash_map_remove(&world->chunks, &position);
    atomic_store(&chunk->visible, false);
    atomic_store(&chunk->unloaded, true);
}

struct world_update_chunk_args {
    struct world *world;
    struct thread_pool *workers;
};

void world_update_chunk(void *key, void *value, void *arg) {
    struct vec3i *position = key;
    struct chunk *chunk = value;

    struct world_update_chunk_args *args = arg;
    struct world *world = args->world;
    struct thread_pool *workers = args->workers;

    // TODO: Move to list maybe
    if (atomic_load(&chunk->state) == CHUNK_STATE_NEEDS_MESH) {
        struct chunk *neighbors[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        bool neighbors_terrain_generated = true;

        for (int i = 0; i < 6; i++) {
            struct vec3i neighbor_position =
                vec3i_add(*position, NEIGHBOR_OFFSETS[i]);

            neighbors[i] = hash_map_get(&world->chunks, &neighbor_position);

            if (neighbors[i] &&
                neighbors[i]->state <= CHUNK_STATE_GENERATING_TERRAIN) {
                neighbors_terrain_generated = false;
                break;
            }
        }

        if (!neighbors_terrain_generated) {
            return;
        }

        struct worker_generate_chunk_mesh_args *args =
            malloc(sizeof(struct worker_generate_chunk_mesh_args));
        args->chunk = chunk;
        args->world = world;
        memcpy(args->neighbors, neighbors, sizeof(struct chunk *) * 6);

        atomic_store(&chunk->state, CHUNK_STATE_QUEUED_MESH);
        thread_pool_schedule(workers, worker_generate_chunk_mesh, args);
    }

    // TODO: Move to like update or something
    if (atomic_load(&chunk->state) == CHUNK_STATE_NEEDS_BUFFERS) {
        // No need to lock chunk
        chunk_update_buffers(chunk);
        static bool expected = false;
        atomic_compare_exchange_strong(&chunk->visible, &expected, true);

        atomic_store(&chunk->state, CHUNK_STATE_READY);
    }
}

// Merge update and draw so not multiple iterations
void world_update(struct world *world) {
    struct world_update_chunk_args args = {world, &world->workers};
    hash_map_for_each(&world->chunks, world_update_chunk, &args);
}

void world_draw_chunk(void *key, void *value, void *arg) {
    // TODO: Surely this parsing every time is inefficient
    struct vec3i *position = key;
    struct chunk *chunk = value;

    // if (atomic_load(&chunk->state) == CHUNK_STATE_READY) {
    pthread_mutex_lock(&chunk->lock);
    chunk_draw(chunk);
    pthread_mutex_unlock(&chunk->lock);
    // }
}

void world_draw(struct world *world) {
    texture_bind(&world->tilemap.texture);

    // TODO: Maybe dont use foreach instead access internal data for better
    // performance
    hash_map_for_each(&world->chunks, world_draw_chunk, NULL);
}

enum block_type world_get_block(struct world *world, struct vec3i position) {
    // rename to chunks loaded
    struct vec3i chunk_position = {floor_div(position.x, CHUNK_SIZE_X),
                                   floor_div(position.y, CHUNK_SIZE_Y),
                                   floor_div(position.z, CHUNK_SIZE_Z)};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk) {
        return BLOCK_TYPE_EMPTY;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    enum block_type block = chunk_get_block(chunk, block_chunk_position);

    return block;
}

// use mipmapping
enum block_type world_get_block_safe(struct world *world,
                                     struct vec3i position) {
    // rename to chunks loaded
    struct vec3i chunk_position = {position.x / CHUNK_SIZE_X,
                                   position.y / CHUNK_SIZE_Y,
                                   position.z / CHUNK_SIZE_Z};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk) {
        return BLOCK_TYPE_EMPTY;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    enum block_type block = chunk_get_block_safe(chunk, block_chunk_position);

    return block;
}

void world_set_block(struct world *world, enum block_type type,
                     struct vec3i position) {
    struct vec3i chunk_position = {floor_div(position.x, CHUNK_SIZE_X),
                                   floor_div(position.y, CHUNK_SIZE_Y),
                                   floor_div(position.z, CHUNK_SIZE_Z)};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk) {
        return;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    chunk_set_block(chunk, block_chunk_position, type);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_MESH);
}

void world_set_block_safe(struct world *world, enum block_type type,
                          struct vec3i position) {
    struct vec3i chunk_position = {position.x / CHUNK_SIZE_X,
                                   position.y / CHUNK_SIZE_Y,
                                   position.z / CHUNK_SIZE_Z};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk) {
        return;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    chunk_set_block_safe(chunk, block_chunk_position, type);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_MESH);
}
