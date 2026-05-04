#include "chunk.h"

#include "block.h"
#include "string.h"
#include <stdatomic.h>
#include <threads.h>
#include <unistd.h>
#include "../math/vec3.h"
#include "../util/direction.h"
#include "../util/stopwatch.h"
#include "../util/gl.h"

void chunk_init(struct chunk *chunk, struct vec3i position,
                enum chunk_type type, struct tilemap *tilemap) {
    chunk->visible = false;
    atomic_init(&chunk->blocks, NULL);
    atomic_init(&chunk->unloaded, false);
    atomic_init(&chunk->ref_count, 0);

    atomic_init(&chunk->blocks_state, CHUNK_BLOCKS_STATE_UNGENERATED);
    atomic_init(&chunk->mesh_state, CHUNK_MESH_STATE_UNGENERATED);
    atomic_init(&chunk->buffers_stale, false);

    chunk->position = position;
    chunk->type = type;
    chunk->neighbor_load_count = 0;
    chunk->tilemap = tilemap;
    dynamic_array_init(&chunk->faces, sizeof(struct face));

    vao_init(&chunk->vao);
    vao_bind(&chunk->vao);

    bo_init(&chunk->ssbo, BO_TYPE_STORAGE);
}

void chunk_destroy(struct chunk *chunk) {
    vao_destroy(&chunk->vao);

    dynamic_array_destroy(&chunk->faces);

    free(atomic_load(&chunk->blocks));
}

void chunk_update_buffers(struct chunk *chunk) {
    bo_bind(&chunk->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(struct face) * chunk->faces.element_count,
                 chunk->faces.array, GL_DYNAMIC_DRAW);

    // unsigned long vbo_size = chunk->vertices.element_count * sizeof(float);
    // unsigned long ibo_size =
    //     chunk->indices.element_count * sizeof(unsigned int);
    //
    // vao_bind(&chunk->vao);
    //
    // bo_upload(&chunk->vbo, vbo_size, chunk->vertices.array,
    //           BO_USAGE_STATIC_DRAW);
    // bo_upload(&chunk->ibo, ibo_size, chunk->indices.array,
    //           BO_USAGE_STATIC_DRAW);
    //
    // chunk->indices_count = chunk->indices.element_count;
}

void chunk_draw(struct chunk *chunk, int gl_chunk_position_location) {
    if (!chunk->visible) {
        return;
    }

    tilemap_bind(chunk->tilemap);
    vao_bind(&chunk->vao);

    bo_bind(&chunk->ssbo);
    // TODO: Don't use raw opengl calls
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, chunk->ssbo.gl_id);

    GL_CALL(glUniform3f(
        gl_chunk_position_location, chunk->position.x * CHUNK_SIZE_X,
        chunk->position.y * CHUNK_SIZE_Y, chunk->position.z * CHUNK_SIZE_Z));

    // TODO: Use triangle strip?
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, chunk->faces.element_count);
}

bool is_block_face_active(struct chunk *chunk, struct chunk **neighbors,
                          struct vec3i position, enum block_face face) {
    switch (face) {
    case BLOCK_FACE_LEFT:
        if (position.x > 0) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x - 1, position.y, position.z}));
        } else {
            return !neighbors[DIRECTION_LEFT] ||
                   !block_is_solid(
                       chunk_get_block(neighbors[DIRECTION_LEFT],
                                       (struct vec3i){CHUNK_SIZE_X - 1,
                                                      position.y, position.z}));
        }
    case BLOCK_FACE_RIGHT:
        if (position.x < CHUNK_SIZE_X - 1) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x + 1, position.y, position.z}));
        } else {
            return !neighbors[DIRECTION_RIGHT] ||
                   !block_is_solid(chunk_get_block(
                       neighbors[DIRECTION_RIGHT],
                       (struct vec3i){0, position.y, position.z}));
        }
    case BLOCK_FACE_BOTTOM:
        if (position.y > 0) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x, position.y - 1, position.z}));
        } else {
            return !neighbors[DIRECTION_BOTTOM] ||
                   !block_is_solid(chunk_get_block(
                       neighbors[DIRECTION_BOTTOM],
                       (struct vec3i){position.x, CHUNK_SIZE_Y - 1,
                                      position.z}));
        }
    case BLOCK_FACE_TOP:
        if (position.y < CHUNK_SIZE_Y - 1) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x, position.y + 1, position.z}));
        } else {
            return !neighbors[DIRECTION_TOP] ||
                   !block_is_solid(chunk_get_block(
                       neighbors[DIRECTION_TOP],
                       (struct vec3i){position.x, 0, position.z}));
        }
    case BLOCK_FACE_BACK:
        if (position.z > 0) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x, position.y, position.z - 1}));
        } else {
            return !neighbors[DIRECTION_BACK] ||
                   !block_is_solid(
                       chunk_get_block(neighbors[DIRECTION_BACK],
                                       (struct vec3i){position.x, position.y,
                                                      CHUNK_SIZE_Z - 1}));
        }
    case BLOCK_FACE_FRONT:
        if (position.z < CHUNK_SIZE_Z - 1) {
            return !block_is_solid(chunk_get_block(
                chunk, (struct vec3i){position.x, position.y, position.z + 1}));
        } else {
            return !neighbors[DIRECTION_FRONT] ||
                   !block_is_solid(chunk_get_block(
                       neighbors[DIRECTION_FRONT],
                       (struct vec3i){position.x, position.y, 0}));
        }
    }
}

void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors) {
    dynamic_array_clear(&chunk->faces);

    struct vec3i chunk_position = vec3i_dot_product(
        chunk->position,
        (struct vec3i){CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z});

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < CHUNK_SIZE_X; x++) {
                struct vec3i position = {x, y, z};
                enum block_type type = chunk_get_block(chunk, position);

                if (type == BLOCK_TYPE_EMPTY) {
                    continue;
                }

                struct vec3i block_position =
                    vec3i_add(position, chunk_position);

                for (int f = 0; f < 6; f++) {
                    if (!is_block_face_active(chunk, neighbors, position, f)) {
                        continue;
                    }

                    struct face new_face;

                    unsigned int tile_index =
                        block_get_face_tile_index(type, f);

                    new_face.data = (x << 28) | (y << 24) | (z << 20) |
                                    (f << 17) | tile_index;

                    dynamic_array_insert_end(&chunk->faces, &new_face);
                }
            }
        }
    }
}

inline enum block_type chunk_get_block(struct chunk *chunk,
                                       struct vec3i position) {
    if (atomic_load(&chunk->blocks_state) != CHUNK_BLOCKS_STATE_GENERATED) {
        return BLOCK_TYPE_EMPTY;
    }

    return atomic_load(atomic_load(&chunk->blocks) + position.x +
                       position.y * CHUNK_SIZE_X +
                       position.z * CHUNK_SIZE_X * CHUNK_SIZE_Y);
}

inline void chunk_set_block(struct chunk *chunk, struct vec3i position,
                            enum block_type type) {
    if (atomic_load(&chunk->blocks_state) != CHUNK_BLOCKS_STATE_GENERATED) {
        return;
    }

    atomic_store(atomic_load(&chunk->blocks) + position.x +
                     position.y * CHUNK_SIZE_X +
                     position.z * CHUNK_SIZE_X * CHUNK_SIZE_Y,
                 type);

    atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_NEEDED);
}
