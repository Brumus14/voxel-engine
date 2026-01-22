#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"
#include "../data_structures/linked_list.h"
#include "../data_structures/hash_map.h"
#include "../util/thread_pool.h"

#define WORLD_WORKER_COUNT 12
#define CHUNKS_BUCKET_COUNT 1024

struct world {
    struct hash_map chunks;
    pthread_rwlock_t chunks_lock; // Is this needed
    struct tilemap tilemap;
    float seed;
    struct thread_pool workers;
};

void world_init(struct world *world);
void world_load_chunk(struct world *world, struct vec3i position);
void world_unload_chunk(struct world *world, struct vec3i position);
void world_draw(struct world *world);
enum block_type world_get_block(struct world *world, struct vec3i position);
enum block_type world_get_block_safe(struct world *world,
                                     struct vec3i position);
void world_set_block(struct world *world, enum block_type type,
                     struct vec3i position);
void world_set_block_safe(struct world *world, enum block_type type,
                          struct vec3i position);

#endif
