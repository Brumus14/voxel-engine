#include "world.h"

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

unsigned int chunk_hasher(void *key) {
    struct vec3i *position = key;
    unsigned int hx = position->x * 0x9E3779B185EBCA87;
    unsigned int hy = position->y * 0xC2B2AE3D27D4EB4F;
    unsigned int hz = position->z * 0x165667B19E3779F9;
    return hx ^ hy ^ hz;
}

void world_init(struct world *world) {
    hash_map_init(&world->chunks, CHUNKS_BUCKET_COUNT, sizeof(struct vec3i),
                  sizeof(struct chunk), chunk_hasher);

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
    chunk_init(new_chunk, position, &world->tilemap);
    atomic_store(&new_chunk->visible, false);

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

struct world_draw_chunk_args {
    struct world *world;
    struct thread_pool *workers;
};

void world_draw_chunk(void *key, void *value, void *arg) {
    // TODO: Surely this parsing every time is inefficient
    struct vec3i *position = key;
    struct chunk *chunk = value;

    struct world_draw_chunk_args *args = arg;
    struct world *world = args->world;
    struct thread_pool *workers = args->workers;

    // TODO: Clean this up and maybe optimise
    bool neighbor_terrain_generated = true;

    // clang-format off
    static const struct vec3i neighbor_offsets[6] = {
        { 1,  0,  0},
        {-1,  0,  0},
        { 0,  1,  0},
        { 0, -1,  0},
        { 0,  0,  1},
        { 0,  0, -1},
    };
    // clang-format on

    // for (int i = 0; i < 6; i++) {
    //     struct vec3i neighbor_position = {
    //         position->x + neighbor_offsets[i].x,
    //         position->y + neighbor_offsets[i].y,
    //         position->z + neighbor_offsets[i].z,
    //     };
    //
    //     struct struct chunk *neighbor =
    //         hash_map_get(&world->chunks, &neighbor_position);
    //
    //     if (neighbor && neighbor->state <= CHUNK_STATE_GENERATING_TERRAIN) {
    //         neighbor_terrain_generated = false;
    //     }
    // }

    // TODO: Move to list maybe
    enum chunk_state chunk_state_needs_mesh = CHUNK_STATE_NEEDS_MESH;

    if (atomic_compare_exchange_strong(&chunk->state, &chunk_state_needs_mesh,
                                       CHUNK_STATE_QUEUED_MESH)) {
        struct worker_generate_chunk_mesh_args *args =
            malloc(sizeof(struct worker_generate_chunk_mesh_args));
        args->chunk = chunk;
        args->world = world;

        thread_pool_schedule(workers, worker_generate_chunk_mesh, args);
    }

    // TODO: Move to like update or something
    if (atomic_load(&chunk->state) == CHUNK_STATE_NEEDS_BUFFERS) {
        pthread_mutex_lock(&chunk->lock); // TODO: Is this necessary?
        chunk_update_buffers(chunk);
        pthread_mutex_unlock(&chunk->lock);

        atomic_store(&chunk->state, CHUNK_STATE_READY);
    }

    if (atomic_load(&chunk->state) == CHUNK_STATE_READY) {
        pthread_mutex_lock(&chunk->lock);
        chunk_draw(chunk);
        pthread_mutex_unlock(&chunk->lock);
    }
}

void world_draw(struct world *world) {
    texture_bind(&world->tilemap.texture);

    struct world_draw_chunk_args args = {world, &world->workers};
    // TODO: Maybe dont use foreach instead access internal data for better
    // performance
    hash_map_for_each(&world->chunks, world_draw_chunk, &args);
}

enum block_type world_get_block(struct world *world, struct vec3i position) {
    // rename to chunks loaded
    struct vec3i chunk_position = {position.x / CHUNK_SIZE_X,
                                   position.y / CHUNK_SIZE_Y,
                                   position.z / CHUNK_SIZE_Z};
    vec3i_print(position);
    vec3i_print(chunk_position);

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

    chunk_set_block(chunk, block_chunk_position, type);
    chunk_generate_mesh(chunk);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_BUFFERS);
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
    chunk_generate_mesh(chunk);

    atomic_store(&chunk->state, CHUNK_STATE_NEEDS_BUFFERS);
}
