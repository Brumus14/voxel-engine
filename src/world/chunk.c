#include "chunk.h"

#include "block.h"
#include "string.h"
#include <stdatomic.h>
#include <threads.h>
#include <unistd.h>
#include "../math/vec3.h"
#include "../util/direction.h"
#include "../util/stopwatch.h"

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
    dynamic_array_init(&chunk->vertices, sizeof(float));
    dynamic_array_init(&chunk->indices, sizeof(unsigned int));

    vao_init(&chunk->vao);
    bo_init(&chunk->vbo, BO_TYPE_VERTEX);
    bo_init(&chunk->ibo, BO_TYPE_INDEX);

    vao_bind(&chunk->vao);
    bo_bind(&chunk->vbo);
    bo_bind(&chunk->ibo);

    vao_attrib(&chunk->vao, 0, 3, VAO_TYPE_FLOAT, false,
               CHUNK_VERTEX_SIZE * sizeof(float), 0);
    vao_attrib(&chunk->vao, 1, 2, VAO_TYPE_FLOAT, false,
               CHUNK_VERTEX_SIZE * sizeof(float), (void *)(3 * sizeof(float)));
    vao_attrib(&chunk->vao, 2, 3, VAO_TYPE_FLOAT, false,
               CHUNK_VERTEX_SIZE * sizeof(float), (void *)(5 * sizeof(float)));

    chunk->indices_count = 0;
}

void chunk_destroy(struct chunk *chunk) {
    bo_destroy(&chunk->ibo);
    bo_destroy(&chunk->vbo);
    vao_destroy(&chunk->vao);

    dynamic_array_destroy(&chunk->indices);
    dynamic_array_destroy(&chunk->vertices);

    free(atomic_load(&chunk->blocks));
}

void chunk_update_buffers(struct chunk *chunk) {
    unsigned long vbo_size = chunk->vertices.element_count * sizeof(float);
    unsigned long ibo_size =
        chunk->indices.element_count * sizeof(unsigned int);

    vao_bind(&chunk->vao);

    bo_upload(&chunk->vbo, vbo_size, chunk->vertices.array,
              BO_USAGE_STATIC_DRAW);
    bo_upload(&chunk->ibo, ibo_size, chunk->indices.array,
              BO_USAGE_STATIC_DRAW);

    chunk->indices_count = chunk->indices.element_count;
}

void chunk_draw(struct chunk *chunk) {
    if (!chunk->visible) {
        return;
    }

    tilemap_bind(chunk->tilemap);
    vao_bind(&chunk->vao);

    renderer_draw_elements(DRAW_MODE_TRIANGLES, chunk->indices_count,
                           INDEX_TYPE_UNSIGNED_INT);
}

bool is_block_face_active(struct chunk *chunk, struct chunk **neighbors,
                          struct vec3i position, enum block_face face) {
    switch (face) {
    case BLOCK_FACE_LEFT:
        if (position.x > 0) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x - 1, position.y,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_LEFT] ||
                   chunk_get_block(neighbors[DIRECTION_LEFT],
                                   (struct vec3i){CHUNK_SIZE_X - 1, position.y,
                                                  position.z}) ==
                       BLOCK_TYPE_EMPTY;
        }
    case BLOCK_FACE_RIGHT:
        if (position.x < CHUNK_SIZE_X - 1) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x + 1, position.y,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_RIGHT] ||
                   chunk_get_block(neighbors[DIRECTION_RIGHT],
                                   (struct vec3i){0, position.y, position.z}) ==
                       BLOCK_TYPE_EMPTY;
        }
    case BLOCK_FACE_BOTTOM:
        if (position.y > 0) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x, position.y - 1,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_BOTTOM] ||
                   chunk_get_block(neighbors[DIRECTION_BOTTOM],
                                   (struct vec3i){position.x, CHUNK_SIZE_Y - 1,
                                                  position.z}) ==
                       BLOCK_TYPE_EMPTY;
        }
    case BLOCK_FACE_TOP:
        if (position.y < CHUNK_SIZE_Y - 1) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x, position.y + 1,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_TOP] ||
                   chunk_get_block(neighbors[DIRECTION_TOP],
                                   (struct vec3i){position.x, 0, position.z}) ==
                       BLOCK_TYPE_EMPTY;
        }
    case BLOCK_FACE_BACK:
        if (position.z > 0) {
            return chunk_get_block(chunk, (struct vec3i){position.x, position.y,
                                                         position.z - 1}) ==
                   BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_BACK] ||
                   chunk_get_block(neighbors[DIRECTION_BACK],
                                   (struct vec3i){position.x, position.y,
                                                  CHUNK_SIZE_Z - 1}) ==
                       BLOCK_TYPE_EMPTY;
        }
    case BLOCK_FACE_FRONT:
        if (position.z < CHUNK_SIZE_Z - 1) {
            return chunk_get_block(chunk, (struct vec3i){position.x, position.y,
                                                         position.z + 1}) ==
                   BLOCK_TYPE_EMPTY;
        } else {
            return !neighbors[DIRECTION_FRONT] ||
                   chunk_get_block(neighbors[DIRECTION_FRONT],
                                   (struct vec3i){position.x, position.y, 0}) ==
                       BLOCK_TYPE_EMPTY;
        }
    }
}

void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors) {
    dynamic_array_clear(&chunk->vertices);
    dynamic_array_clear(&chunk->indices);

    struct vec3i chunk_position = vec3i_dot_product(
        chunk->position,
        (struct vec3i){CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z});

    int faces_added = 0;

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

                    struct rectangle texture_rectangle =
                        tilemap_get_tile_rectangle(
                            chunk->tilemap, block_type_to_texture(type)
                                                .face_texture_indices[f]);

                    for (int i = 0; i < 4; i++) {
                        float vertex_position[3] = {
                            block_position.x +
                                VERTEX_OFFSETS[FACE_INDICES[f][i]][0],
                            block_position.y +
                                VERTEX_OFFSETS[FACE_INDICES[f][i]][1],
                            block_position.z +
                                VERTEX_OFFSETS[FACE_INDICES[f][i]][2],
                        };

                        dynamic_array_insert_end(&chunk->vertices,
                                                 &vertex_position[0]);
                        dynamic_array_insert_end(&chunk->vertices,
                                                 &vertex_position[1]);
                        dynamic_array_insert_end(&chunk->vertices,
                                                 &vertex_position[2]);

                        float texture_coordinates[2] = {
                            texture_rectangle.x + texture_rectangle.width *
                                                      TEXTURE_COORDINATES[i][0],
                            texture_rectangle.y + texture_rectangle.height *
                                                      TEXTURE_COORDINATES[i][1],
                        };

                        dynamic_array_insert_end(&chunk->vertices,
                                                 &texture_coordinates[0]);
                        dynamic_array_insert_end(&chunk->vertices,
                                                 &texture_coordinates[1]);

                        dynamic_array_insert_end(&chunk->vertices,
                                                 &FACE_NORMALS[f][0]);
                        dynamic_array_insert_end(&chunk->vertices,
                                                 &FACE_NORMALS[f][1]);
                        dynamic_array_insert_end(&chunk->vertices,
                                                 &FACE_NORMALS[f][2]);
                    }

                    for (int j = 0; j < 6; j++) {
                        unsigned int index = faces_added * 4 + INDEX_ORDER[j];
                        dynamic_array_insert_end(&chunk->indices, &index);
                    }

                    faces_added++;
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
