#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include "../math/vec3.h"
#include "../graphics/graphics.h"
#include "../graphics/tilemap.h"
#include <pthread.h>
#include <stdatomic.h>

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

#define CHUNK_VERTEX_SIZE 8

// Rename to terrain?
enum chunk_blocks_state {
    CHUNK_BLOCKS_STATE_UNGENERATED = 0,
    CHUNK_BLOCKS_STATE_NEEDED,
    CHUNK_BLOCKS_STATE_QUEUED,
    CHUNK_BLOCKS_STATE_GENERATING,
    CHUNK_BLOCKS_STATE_GENERATED,
};

// Rename needed to stale?
// Rename ungenerated
enum chunk_mesh_state {
    CHUNK_MESH_STATE_UNGENERATED = 0,
    CHUNK_MESH_STATE_NEEDED,
    CHUNK_MESH_STATE_QUEUED,
    CHUNK_MESH_STATE_GENERATING,
    CHUNK_MESH_STATE_GENERATED,
};

// Order structs for packing
struct chunk {
    atomic_bool visible;
    atomic_bool unloaded; // Is there better name
    _Atomic(enum chunk_blocks_state) blocks_state;
    _Atomic(enum chunk_mesh_state) mesh_state;
    atomic_bool buffers_stale;
    atomic_int ref_count;
    pthread_mutex_t lock;
    struct vec3i position;
    // Read write lock?
    enum block_type *blocks;
    struct tilemap *tilemap;
    float *vertices;
    unsigned int *indices;
    int face_count;
    struct bo vbo;
    struct bo ibo;
    struct vao vao;
};

// What even are these
// clang-format off
static const float VERTEX_POSITIONS[8][3] = {
    {0, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 1, 1},
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0},
};

static const unsigned int FACE_INDICES[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {1, 5, 6, 2},
    {4, 5, 1, 0},
    {4, 0, 3, 7},
    {5, 4, 7, 6},
};

static const float FACE_NORMALS[6][3] = {
    { 0,  0,  1},
    { 0,  1,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    {-1,  0,  0},
    { 0,  0, -1},
};

static const int INDEX_ORDER[6] = {0, 1, 2,
                                   0, 2, 3};

// Order specified in direction.h
static const struct vec3i NEIGHBOR_OFFSETS[6] = {
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1},
};
// clang-format on

void chunk_init(struct chunk *chunk, struct vec3i position,
                struct tilemap *tilemap);
void chunk_destroy(struct chunk *chunk);
void chunk_update_buffers(struct chunk *chunk);
void chunk_draw(struct chunk *chunk);
void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors);
// Position be pointer?
enum block_type chunk_get_block(struct chunk *chunk, struct vec3i position);
void chunk_set_block(struct chunk *chunk, struct vec3i position,
                     enum block_type type);

#endif
