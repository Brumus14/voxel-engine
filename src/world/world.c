#include "world.h"

#include <math.h>
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
#include "../util/log.h"
#include "../util/direction.h"
#include "../util/gl.h"

// Hash function from:
// https://matthias-research.github.io/pages/publications/tetraederCollision.pdf
unsigned int chunks_hash_function(void *key) {
    struct vec3i *position = key;
    unsigned int hx = position->x * 73856093;
    unsigned int hy = position->y * 19349663;
    unsigned int hz = position->z * 83492791;
    return hx ^ hy ^ hz;
}

static inline bool chunks_key_compare_function(void *key_a, void *key_b) {
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

    world->seed = random_int();

    thread_pool_init(&world->workers, WORLD_WORKER_COUNT);

    shader_program_from_files(&world->shader_program, "res/shaders/voxel.vert",
                              "res/shaders/voxel.frag");
    shader_program_bind_attribute(&world->shader_program, 0, "position");
    shader_program_link(&world->shader_program);

    shader_program_use(&world->shader_program);

    // TODO: Abstract opengl calls
    world->gl_chunk_position_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "chunk_position"),
        GLint);
    world->gl_tilemap_width_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "tilemap_width"),
        GLint);
    world->gl_tilemap_height_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "tilemap_height"),
        GLint);
    world->gl_tilemap_tiles_width_location =
        GL_CALL_R(glGetUniformLocation(world->shader_program.gl_id,
                                       "tilemap_tiles_width"),
                  GLint);
    world->gl_tilemap_tiles_height_location =
        GL_CALL_R(glGetUniformLocation(world->shader_program.gl_id,
                                       "tilemap_tiles_height"),
                  GLint);
    world->gl_tilemap_tile_width_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "tilemap_tile_width"),
        GLint);
    world->gl_tilemap_tile_height_location =
        GL_CALL_R(glGetUniformLocation(world->shader_program.gl_id,
                                       "tilemap_tile_height"),
                  GLint);
    world->gl_tilemap_margin_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "tilemap_margin"),
        GLint);
    world->gl_tilemap_spacing_location = GL_CALL_R(
        glGetUniformLocation(world->shader_program.gl_id, "tilemap_spacing"),
        GLint);

    GL_CALL(
        glUniform1ui(world->gl_tilemap_width_location, world->tilemap.width));
    GL_CALL(
        glUniform1ui(world->gl_tilemap_height_location, world->tilemap.height));
    GL_CALL(glUniform1ui(world->gl_tilemap_tiles_width_location,
                         world->tilemap.tiles_width));
    GL_CALL(glUniform1ui(world->gl_tilemap_tiles_height_location,
                         world->tilemap.tiles_height));
    GL_CALL(glUniform1ui(world->gl_tilemap_tile_width_location,
                         world->tilemap.tile_width));
    GL_CALL(glUniform1ui(world->gl_tilemap_tile_height_location,
                         world->tilemap.tile_height));
    GL_CALL(
        glUniform1ui(world->gl_tilemap_margin_location, world->tilemap.margin));
    GL_CALL(glUniform1ui(world->gl_tilemap_spacing_location,
                         world->tilemap.spacing));
}

void world_destroy(struct world *world) {
    shader_program_destroy(&world->shader_program);
    thread_pool_destroy(&world->workers);
    tilemap_destroy(&world->tilemap);
    dynamic_array_destroy(&world->unloaded_chunks);
    hash_map_destroy(&world->chunks);
}

void load_chunk(struct world *world, struct vec3i position,
                enum chunk_type type, bool neighbor_load) {
    // Check not already loaded
    struct chunk *chunk = hash_map_get(&world->chunks, &position);

    if (chunk) {
        if (chunk->type > type) {
            chunk->type = type;

            if (neighbor_load) {
                chunk->neighbor_load_count++;
            }
        }

        return;
    }

    struct chunk *new_chunk = malloc(sizeof(struct chunk));
    chunk_init(new_chunk, position, type, &world->tilemap);

    if (neighbor_load) {
        new_chunk->neighbor_load_count++;
    }

    hash_map_put(&world->chunks, &new_chunk->position, new_chunk);

    if (type == CHUNK_TYPE_FULL) {
        WORLD_LOG({
            printf("Loaded ");
            vec3i_print(position);
            putchar('\n');
        });
    }

    atomic_store(&new_chunk->blocks_state, CHUNK_BLOCKS_STATE_NEEDED);
}

void world_load_chunk(struct world *world, struct vec3i position) {
    load_chunk(world, position, CHUNK_TYPE_FULL, false);

    for (int i = 0; i < 6; i++) {
        struct vec3i neighbor_position = vec3i_add(position, DIRECTIONS[i]);

        load_chunk(world, neighbor_position, CHUNK_TYPE_TERRAIN, true);
    }
}

// remove chunks from chunks to generate if have moved out of render distance
// before generated
void world_unload_chunk(struct world *world, struct vec3i position) {
    // Store in unloaded list maybe?
    // struct chunk *chunk = hash_map_remove(&world->chunks, &position);
    struct chunk *chunk = hash_map_get(&world->chunks, &position);

    if (!chunk || chunk->type != CHUNK_TYPE_FULL) {
        return;
    }

    WORLD_LOG({
        printf("Unloading ");
        vec3i_print(position);
        putchar('\n');
    });

    atomic_store(&chunk->unloaded, true);

    for (int i = 0; i < 6; i++) {
        struct vec3i neighbor_position = vec3i_add(position, DIRECTIONS[i]);

        struct chunk *neighbor =
            hash_map_get(&world->chunks, &neighbor_position);

        if (!neighbor) {
            continue;
        }

        neighbor->neighbor_load_count--;

        if (neighbor->type == CHUNK_TYPE_TERRAIN &&
            neighbor->neighbor_load_count == 0) {
            atomic_store(&neighbor->unloaded, true);
        }
    }
}

bool get_chunk_neighbors(struct world *world, struct vec3i position,
                         struct chunk **neighbors) {
    for (int i = 0; i < 6; i++) {
        struct vec3i neighbor_position = vec3i_add(position, DIRECTIONS[i]);

        neighbors[i] = hash_map_get(&world->chunks, &neighbor_position);

        if (!neighbors[i]) {
            // This shouldn't be reachable
            return false;
        }

        if (atomic_load(&neighbors[i]->unloaded)) {
            neighbors[i] = NULL;
            continue;
        }

        if (atomic_load(&neighbors[i]->blocks_state) !=
            CHUNK_BLOCKS_STATE_GENERATED) {
            return false;
        }
    }

    return true;
}

struct world_update_chunk_args {
    struct world *world;
    struct thread_pool *workers;
    struct vec3d player_position;
};

static inline void world_update_chunk(struct world *world, struct chunk *chunk,
                                      struct vec3d player_position) {
    if (atomic_load(&chunk->unloaded)) {
        if (atomic_load(&chunk->ref_count) == 0) {
            dynamic_array_insert_end(&world->unloaded_chunks, &chunk->position);
        }

        return;
    }

    // Maybe move this inside chunk
    struct vec3i chunk_block_position = vec3i_dot_product(
        chunk->position,
        (struct vec3i){CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z});

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
        float priority =
            sqrtf(pow(chunk_block_position.x - player_position.x, 2) +
                  pow(chunk_block_position.y - player_position.y, 2) +
                  pow(chunk_block_position.z - player_position.z, 2));
        thread_pool_schedule(&world->workers, worker_generate_chunk_terrain,
                             args, priority);

        WORLD_LOG({
            printf("Queued terrain ");
            vec3i_print(chunk->position);
            putchar('\n');
        });
    }

    if (chunk->type == CHUNK_TYPE_FULL &&
        atomic_load(&chunk->blocks_state) == CHUNK_BLOCKS_STATE_GENERATED) {
        expected_mesh_state = CHUNK_MESH_STATE_UNGENERATED;
        atomic_compare_exchange_strong(&chunk->mesh_state, &expected_mesh_state,
                                       CHUNK_MESH_STATE_NEEDED);
    }

    // TODO: Move to lists maybe
    expected_mesh_state = CHUNK_MESH_STATE_NEEDED;
    // Is CAS needed since mostly on main thread
    if (atomic_compare_exchange_strong(&chunk->mesh_state, &expected_mesh_state,
                                       CHUNK_MESH_STATE_QUEUED)) {
        struct chunk *neighbors[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        bool neighbors_valid =
            get_chunk_neighbors(world, chunk->position, neighbors);

        if (!neighbors_valid) {
            atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_NEEDED);
        } else {
            struct worker_generate_chunk_mesh_args *args =
                malloc(sizeof(struct worker_generate_chunk_mesh_args));
            args->chunk = chunk;
            args->world = world;
            memcpy(args->neighbors, neighbors, sizeof(struct chunk *) * 6);

            for (int i = 0; i < 6; i++) {
                if (neighbors[i]) {
                    atomic_fetch_add(&neighbors[i]->ref_count, 1);
                }
            }

            atomic_fetch_add(&chunk->ref_count, 1);
            float priority =
                sqrtf(pow(chunk_block_position.x - player_position.x, 2) +
                      pow(chunk_block_position.y - player_position.y, 2) +
                      pow(chunk_block_position.z - player_position.z, 2));
            thread_pool_schedule(&world->workers, worker_generate_chunk_mesh,
                                 args, priority);

            WORLD_LOG({
                printf("Queued mesh ");
                vec3i_print(chunk->position);
                putchar('\n');
            });
        }
    }

    bool expected_buffers_stale = true;
    if (atomic_load(&chunk->mesh_state) == CHUNK_MESH_STATE_GENERATED &&
        atomic_compare_exchange_strong(&chunk->buffers_stale,
                                       &expected_buffers_stale, false)) {
        chunk_update_buffers(chunk);

        chunk->visible = true;
    }
}

// Merge update and draw so not multiple iterations
// It seems really inefficient to iterate over all chunks?
void world_update(struct world *world, struct vec3d player_position) {
    dynamic_array_clear(&world->unloaded_chunks);

    unsigned int i = 0;
    struct hash_map_node *node = NULL;

    while (hash_map_iterate(&world->chunks, &i, &node)) {
        world_update_chunk(world, node->value, player_position);
    }

    for (unsigned long i = 0; i < world->unloaded_chunks.element_count; i++) {
        struct vec3i *position = dynamic_array_get(&world->unloaded_chunks, i);
        struct chunk *chunk = hash_map_remove(&world->chunks, position);

        WORLD_LOG({
            printf("Unloaded ");
            vec3i_print(chunk->position);
            putchar('\n');
        });

        chunk_destroy(chunk);
        free(chunk);
    }
}

void world_prepare_draw(struct world *world) {
    texture_bind(&world->tilemap.texture);
    shader_program_use(&world->shader_program);
}

void world_draw(struct world *world) {
    unsigned int i = 0;
    struct hash_map_node *node = NULL;

    while (hash_map_iterate(&world->chunks, &i, &node)) {
        chunk_draw(node->value, world->gl_chunk_position_location);
    }
}

// use mipmapping
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

    return chunk_get_block(chunk, block_chunk_position);
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

    // Change to invalid then worker checks if neighbor needs regenerating too
    // TODO: This is very messy
    struct vec3i neighbor_position;
    struct chunk *neighbor = NULL;

    if (block_chunk_position.x == 0) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_LEFT]);
    } else if (block_chunk_position.x == CHUNK_SIZE_X - 1) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_RIGHT]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
    neighbor = NULL;

    if (block_chunk_position.y == 0) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_BOTTOM]);
    } else if (block_chunk_position.y == CHUNK_SIZE_Y - 1) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_TOP]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
    neighbor = NULL;

    if (block_chunk_position.z == 0) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_BACK]);
    } else if (block_chunk_position.z == CHUNK_SIZE_Z - 1) {
        neighbor_position =
            vec3i_add(chunk_position, DIRECTIONS[DIRECTION_FRONT]);
    }

    neighbor = hash_map_get(&world->chunks, &neighbor_position);
    if (neighbor) {
        atomic_store(&neighbor->mesh_state, CHUNK_MESH_STATE_NEEDED);
    }
}
