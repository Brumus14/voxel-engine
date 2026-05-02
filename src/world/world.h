#ifndef WORLD_H
#define WORLD_H

#include "../data_structures/hash_map.h"
#include "../data_structures/dynamic_array.h"
#include "../util/thread_pool.h"
#include "../math/vec3.h"
#include "../graphics/tilemap.h"
#include "chunk.h"

#define WORLD_WORKER_COUNT 12
#define CHUNKS_BUCKET_COUNT 1024

struct world {
    struct hash_map chunks;
    // Rename to chunks to remove
    struct dynamic_array unloaded_chunks;
    struct tilemap tilemap;
    int seed;
    struct thread_pool workers;
    struct shader_program shader_program;

    int gl_chunk_position_location;
};

void world_init(struct world *world);
void world_destroy(struct world *world);
void world_load_chunk(struct world *world, struct vec3i position);
void world_unload_chunk(struct world *world, struct vec3i position);
void world_update(struct world *world, struct vec3d player_position);
void world_prepare_draw(struct world *world);
void world_draw(struct world *world);
enum block_type world_get_block(struct world *world, struct vec3i position);
void world_set_block(struct world *world, enum block_type type,
                     struct vec3i position);

#endif
