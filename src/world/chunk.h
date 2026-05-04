#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include "../math/vec3.h"
#include "../graphics/graphics.h"
#include "../graphics/tilemap.h"
#include <pthread.h>
#include <stdatomic.h>
#include "../data_structures/dynamic_array.h"
#include "../util/direction.h"

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

enum chunk_type {
    CHUNK_TYPE_FULL = 0,
    CHUNK_TYPE_TERRAIN,
};

struct face {
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int direction;
    unsigned int tile_index;
};

struct chunk {
    bool visible;
    _Atomic(_Atomic(enum block_type) *) blocks;
    atomic_bool unloaded;
    atomic_int ref_count;

    _Atomic(enum chunk_blocks_state) blocks_state;
    _Atomic(enum chunk_mesh_state) mesh_state;
    atomic_bool buffers_stale;

    struct vec3i position;
    enum chunk_type type;
    int neighbor_load_count;
    struct tilemap *tilemap;
    struct dynamic_array faces;

    // Abstract into mesh?
    struct vao vao;
    struct bo ssbo;
};

// clang-format off
static const float VERTEX_OFFSETS[8][3] = {
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {1, 1, 0},
    {0, 0, 1},
    {1, 0, 1},
    {0, 1, 1},
    {1, 1, 1},
};

static const float TEXTURE_COORDINATES[4][2] = {
    {0, 1},
    {1, 1},
    {1, 0},
    {0, 0},
};

static const float FACE_NORMALS[6][3] = {
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1},
};

// The indices of vertex offsets for a face
static const unsigned int FACE_INDICES[6][4] = {
    {0, 4, 6, 2},
    {5, 1, 3, 7},
    {0, 1, 5, 4},
    {6, 7, 3, 2},
    {1, 0, 2, 3},
    {4, 5, 7, 6},
};

static const int INDEX_ORDER[6] = {0, 1, 2,
                                   0, 2, 3};
// clang-format on

void chunk_init(struct chunk *chunk, struct vec3i position,
                enum chunk_type type, struct tilemap *tilemap);
void chunk_destroy(struct chunk *chunk);
void chunk_update_buffers(struct chunk *chunk);
void chunk_draw(struct chunk *chunk, int gl_chunk_position_location);
void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors);
enum block_type chunk_get_block(struct chunk *chunk, struct vec3i position);
void chunk_set_block(struct chunk *chunk, struct vec3i position,
                     enum block_type type);

#endif
