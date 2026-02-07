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
#include "../util/direction.h"
#include "../util/logging.h"

// Hash function from:
// https://matthias-research.github.io/pages/publications/tetraederCollision.pdf
unsigned int chunks_hash_function(void *key) {
    struct vec3i *position = key;
    unsigned int hx = position->x * 73856093;
    unsigned int hy = position->y * 19349663;
    unsigned int hz = position->z * 83492791;
    return hx ^ hy ^ hz;
}

bool chunks_key_compare_function(void *key_a, void *key_b) {
    struct vec3i *a = key_a;
    struct vec3i *b = key_b;

    return (a->x == b->x && a->y == b->y && a->z == b->z);
}

void world_init(struct world *world) {
    hash_map_init(&world->chunks, CHUNKS_BUCKET_COUNT, sizeof(struct vec3i),
                  sizeof(struct chunk), chunks_hash_function,
                  chunks_key_compare_function);

    // Use vec3i pointer instead?
    dynamic_array_init(&world->unloaded_chunks, sizeof(struct vec3i));

    tilemap_init(&world->tilemap, "res/textures/atlas.png",
                 TEXTURE_FILTER_NEAREST, 16, 16, 1, 2);

    // More seeds
    world->seed = random_range(0, 100);

    thread_pool_init(&world->workers, WORLD_WORKER_COUNT);
}

void world_destroy(struct world *world) {
    thread_pool_destroy(&world->workers);
    tilemap_destroy(&world->tilemap);
    hash_map_destroy(&world->chunks);
}

void world_load_chunk(struct world *world, struct vec3i position) {
    // Check not already loaded
    if (hash_map_get(&world->chunks, &position)) {
        return;
    }

    WORLD_LOG({
        // Improve logging messages
        printf("added ");
        vec3i_print(position);
        putchar('\n');
    });

    struct chunk *new_chunk = malloc(sizeof(struct chunk));
    chunk_init(new_chunk, position, &world->tilemap);

    struct chunk *old_value =
        hash_map_put(&world->chunks, &position, new_chunk);
    free(old_value);

    atomic_store(&new_chunk->blocks_state, CHUNK_BLOCKS_STATE_NEEDED);
}

// remove chunks from chunks to generate if have moved out of render distance
// before generated
void world_unload_chunk(struct world *world, struct vec3i position) {
    // Store in unloaded list maybe?
    // struct chunk *chunk = hash_map_remove(&world->chunks, &position);
    struct chunk *chunk = hash_map_get(&world->chunks, &position);

    if (!chunk) {
        return;
    }

    atomic_store(&chunk->unloaded, true);

    WORLD_LOG({
        printf("unloading ");
        vec3i_print(position);
        putchar('\n');
    });

    // Can i just remove
    // struct chunk *chunk = hash_map_get(&world->chunks, &position);
    // atomic_store(&chunk->state, CHUNK_STATE_UNLOADED);
    // Can i add to array immediately
    // dynamic_array_insert_end(&world->unloaded_chunks, &position);
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

    // Use switches
    if (atomic_load(&chunk->unloaded) && atomic_load(&chunk->ref_count) == 0) {
        dynamic_array_insert_end(&world->unloaded_chunks, position);
        return;
    }

    enum chunk_blocks_state expected_blocks_state;
    enum chunk_mesh_state expected_mesh_state;

    // Rename
    expected_blocks_state = CHUNK_BLOCKS_STATE_NEEDED;
    if (atomic_compare_exchange_strong(&chunk->blocks_state,
                                       &expected_blocks_state,
                                       CHUNK_BLOCKS_STATE_QUEUED)) {
        struct worker_generate_chunk_terrain_args *args =
            malloc(sizeof(struct worker_generate_chunk_terrain_args));
        args->chunk = chunk;
        args->seed = world->seed;

        atomic_fetch_add(&chunk->ref_count, 1);
        thread_pool_schedule(&world->workers, worker_generate_chunk_terrain,
                             args);
    }

    if (atomic_load(&chunk->blocks_state) == CHUNK_BLOCKS_STATE_GENERATED) {
        expected_mesh_state = CHUNK_MESH_STATE_UNGENERATED;
        atomic_compare_exchange_strong(&chunk->mesh_state, &expected_mesh_state,
                                       CHUNK_MESH_STATE_NEEDED);
    }

    // TODO: Move to list maybe
    expected_mesh_state = CHUNK_MESH_STATE_NEEDED;
    // Is CAS needed since mostly on main thread
    if (atomic_compare_exchange_strong(&chunk->mesh_state, &expected_mesh_state,
                                       CHUNK_MESH_STATE_QUEUED)) {
        // Make get neighbors function?
        struct chunk *neighbors[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        bool neighbors_terrain_generated = true;

        for (int i = 0; i < 6; i++) {
            struct vec3i neighbor_position =
                vec3i_add(*position, NEIGHBOR_OFFSETS[i]);

            neighbors[i] = hash_map_get(&world->chunks, &neighbor_position);

            if (!neighbors[i] || atomic_load(&neighbors[i]->blocks_state) !=
                                     CHUNK_BLOCKS_STATE_GENERATED) {
                neighbors_terrain_generated = false;
                break;
            }
        }

        // vec3i_print(*position);
        // printf(" : %s\n", neighbors_terrain_generated ? "true" : "false");
        // if (neighbors_terrain_generated) {
        //     for (int i = 0; i < 6; i++) {
        //         printf("%d\n", atomic_load(&chunk->mesh_state));
        //     }
        //     printf("end\n");
        // }

        if (!neighbors_terrain_generated) {
            atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_NEEDED);
            return;
        }

        for (int i = 0; i < 6; i++) {
            atomic_fetch_add(&neighbors[i]->ref_count, 1);
        }

        struct worker_generate_chunk_mesh_args *args =
            malloc(sizeof(struct worker_generate_chunk_mesh_args));
        args->chunk = chunk;
        args->world = world;
        memcpy(args->neighbors, neighbors, sizeof(struct chunk *) * 6);

        // Task has reference to chunk
        atomic_fetch_add(&chunk->ref_count, 1);
        thread_pool_schedule(workers, worker_generate_chunk_mesh, args);
    }

    // TODO: Move to like update or something
    bool expected_buffers_stale = true;
    if (atomic_compare_exchange_strong(&chunk->buffers_stale,
                                       &expected_buffers_stale, false)) {
        // No need to lock chunk
        chunk_update_buffers(chunk);
        static bool expected = false;
        atomic_compare_exchange_strong(&chunk->visible, &expected, true);
    }
}

// Merge update and draw so not multiple iterations
// It seems really inefficient to iterate over all chunks?
void world_update(struct world *world) {
    dynamic_array_clear(&world->unloaded_chunks);

    struct world_update_chunk_args args = {world, &world->workers};
    hash_map_for_each(&world->chunks, world_update_chunk, &args);

    for (int i = 0; i < world->unloaded_chunks.element_count; i++) {
        struct vec3i *position = dynamic_array_get(&world->unloaded_chunks, i);
        struct chunk *chunk = hash_map_remove(&world->chunks, position);

        if (atomic_load(&chunk->ref_count) == 0) {
            WORLD_LOG({
                printf("unloaded ");
                vec3i_print(chunk->position);
                putchar('\n');
            });

            chunk_destroy(chunk);
            free(chunk);
        }
    }
}

void world_draw_chunk(void *key, void *value, void *arg) {
    // TODO: Surely this parsing every time is inefficient
    struct vec3i *position = key;
    struct chunk *chunk = value;

    chunk_draw(chunk);
}

void world_draw(struct world *world) {
    texture_bind(&world->tilemap.texture);

    // TODO: Maybe dont use foreach instead access internal data for better
    // performance
    hash_map_for_each(&world->chunks, world_draw_chunk, NULL);
}

// use mipmapping
enum block_type world_get_block(struct world *world, struct vec3i position) {
    // rename to chunks loaded
    struct vec3i chunk_position = {floor_div(position.x, CHUNK_SIZE_X),
                                   floor_div(position.y, CHUNK_SIZE_Y),
                                   floor_div(position.z, CHUNK_SIZE_Z)};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk ||
        atomic_load(&chunk->blocks_state) != CHUNK_BLOCKS_STATE_GENERATED) {
        return BLOCK_TYPE_EMPTY;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    enum block_type block = chunk_get_block(chunk, block_chunk_position);

    return block;
}

void world_set_block(struct world *world, enum block_type type,
                     struct vec3i position) {
    struct vec3i chunk_position = {floor_div(position.x, CHUNK_SIZE_X),
                                   floor_div(position.y, CHUNK_SIZE_Y),
                                   floor_div(position.z, CHUNK_SIZE_Z)};

    struct chunk *chunk = hash_map_get(&world->chunks, &chunk_position);

    if (!chunk ||
        atomic_load(&chunk->blocks_state) != CHUNK_BLOCKS_STATE_GENERATED) {
        return;
    }

    struct vec3i block_chunk_position = {mod(position.x, CHUNK_SIZE_X),
                                         mod(position.y, CHUNK_SIZE_Y),
                                         mod(position.z, CHUNK_SIZE_Z)};

    chunk_set_block(chunk, block_chunk_position, type);

    // Change to invalid then worker checks if neighbor needs regenerating too
    // This is very messy
    struct vec3i neighbor_position;
    struct chunk *neighbor = NULL;

    if (block_chunk_position.x == 0) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_LEFT]);
    } else if (block_chunk_position.x == CHUNK_SIZE_X - 1) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_RIGHT]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
    neighbor = NULL;

    if (block_chunk_position.y == 0) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_BOTTOM]);
    } else if (block_chunk_position.y == CHUNK_SIZE_Y - 1) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_TOP]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
    neighbor = NULL;

    if (block_chunk_position.z == 0) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_BACK]);
    } else if (block_chunk_position.z == CHUNK_SIZE_Z - 1) {
        neighbor_position =
            vec3i_add(chunk_position, NEIGHBOR_OFFSETS[DIRECTION_FRONT]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
}
