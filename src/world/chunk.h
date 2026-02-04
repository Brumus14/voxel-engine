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

enum chunk_state {
    CHUNK_STATE_NEEDS_TERRAIN = 0,
    // CHUNK_STATE_QUEUED_TERRAIN,
    CHUNK_STATE_GENERATING_TERRAIN,
    CHUNK_STATE_NEEDS_MESH,
    CHUNK_STATE_QUEUED_MESH,
    CHUNK_STATE_GENERATING_MESH,
    CHUNK_STATE_NEEDS_BUFFERS,
    CHUNK_STATE_READY,
    CHUNK_STATE_UNLOADED,
};

struct chunk {
    atomic_bool visible;
    _Atomic enum chunk_state state;
    atomic_int ref_count;
    pthread_mutex_t lock;
    struct vec3i position;
    // enum block_type blocks[CHUNK_SIZE_Z * CHUNK_SIZE_Y * CHUNK_SIZE_X];
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
                struct tilemap *tilemap, bool visible);
void chunk_destroy(struct chunk *chunk);
void chunk_update_buffers(struct chunk *chunk);
void chunk_draw(struct chunk *chunk);
void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors);
// Position be pointer?
enum block_type chunk_get_block(struct chunk *chunk, struct vec3i position);
void chunk_set_block(struct chunk *chunk, struct vec3i position,
                     enum block_type type);

#endif
