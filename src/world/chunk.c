#include "chunk.h"

#include "string.h"
#include <threads.h>
#include "../util/direction.h"
#include "../util/stopwatch.h"

void chunk_init(struct chunk *chunk, struct vec3i position,
                struct tilemap *tilemap) {
    chunk->position = position;
    chunk->blocks = NULL;
    atomic_init(&chunk->blocks_state, CHUNK_BLOCKS_STATE_UNGENERATED);
    atomic_init(&chunk->mesh_state, CHUNK_MESH_STATE_UNGENERATED);
    atomic_init(&chunk->buffers_stale, false);
    atomic_init(&chunk->ref_count, 0);
    pthread_mutex_init(&chunk->lock, NULL);
    atomic_init(&chunk->visible, false);
    atomic_init(&chunk->unloaded, false);
    chunk->tilemap = tilemap;

    chunk->vertices = NULL;
    chunk->indices = NULL;
    chunk->face_count = 0;

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
}

void chunk_destroy(struct chunk *chunk) {
    vao_destroy(&chunk->vao);
    bo_destroy(&chunk->vbo);
    bo_destroy(&chunk->ibo);

    pthread_mutex_destroy(&chunk->lock);
    free(chunk->indices);
    free(chunk->vertices);
    free(chunk->blocks);
}

void chunk_update_buffers(struct chunk *chunk) {
    vao_bind(&chunk->vao);
    bo_bind(&chunk->vbo);
    bo_bind(&chunk->ibo);

    bo_upload(&chunk->vbo,
              chunk->face_count * 4 * CHUNK_VERTEX_SIZE * sizeof(float),
              chunk->vertices, BO_USAGE_STATIC_DRAW);

    bo_upload(&chunk->ibo, chunk->face_count * 6 * sizeof(unsigned int),
              chunk->indices, BO_USAGE_STATIC_DRAW);
}

// bind texture
void chunk_draw(struct chunk *chunk) {
    static struct stopwatch s;
    stopwatch_start(&s);
    if (!atomic_load(&chunk->visible) || atomic_load(&chunk->unloaded)) {
        return;
    }

    vao_bind(&chunk->vao);
    bo_bind(&chunk->ibo);
    bo_bind(&chunk->vbo);

    // Potentially inefficient
    int index_count = bo_get_size(&chunk->ibo) / sizeof(unsigned int);

    renderer_draw_elements(DRAW_MODE_TRIANGLES, index_count,
                           INDEX_TYPE_UNSIGNED_INT);
    stopwatch_end(&s);
    // printf("TIMEEEEEE: %f\n", stopwatch_time(&s));
}

// Maybe return all faces to call less times
// Rename to is visible?
// Possibly reduce repetition here
bool is_block_face_active(struct chunk *chunk, struct chunk **neighbors,
                          struct vec3i position, enum block_face face) {
    // Normalise the directional order with like neighbor offset etc
    switch (face) {
    case BLOCK_FACE_LEFT:
        if (position.x > 0) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x - 1, position.y,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else if (position.x == 0 && neighbors[DIRECTION_LEFT]) {
            return chunk_get_block(neighbors[DIRECTION_LEFT],
                                   (struct vec3i){CHUNK_SIZE_X - 1, position.y,
                                                  position.z}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    case BLOCK_FACE_RIGHT:
        if (position.x < CHUNK_SIZE_X - 1) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x + 1, position.y,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else if (position.x == CHUNK_SIZE_X - 1 &&
                   neighbors[DIRECTION_RIGHT]) {
            return chunk_get_block(neighbors[DIRECTION_RIGHT],
                                   (struct vec3i){0, position.y, position.z}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    case BLOCK_FACE_BOTTOM:
        if (position.y > 0) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x, position.y - 1,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else if (position.y == 0 && neighbors[DIRECTION_BOTTOM]) {
            return chunk_get_block(neighbors[DIRECTION_BOTTOM],
                                   (struct vec3i){position.x, CHUNK_SIZE_Y - 1,
                                                  position.z}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    case BLOCK_FACE_TOP:
        if (position.y < CHUNK_SIZE_Y - 1) {
            return chunk_get_block(
                       chunk, (struct vec3i){position.x, position.y + 1,
                                             position.z}) == BLOCK_TYPE_EMPTY;
        } else if (position.y == CHUNK_SIZE_Y - 1 && neighbors[DIRECTION_TOP]) {
            return chunk_get_block(neighbors[DIRECTION_TOP],
                                   (struct vec3i){position.x, 0, position.z}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    case BLOCK_FACE_BACK:
        if (position.z > 0) {
            return chunk_get_block(chunk, (struct vec3i){position.x, position.y,
                                                         position.z - 1}) ==
                   BLOCK_TYPE_EMPTY;
        } else if (position.z == 0 && neighbors[DIRECTION_BACK]) {
            return chunk_get_block(neighbors[DIRECTION_BACK],
                                   (struct vec3i){position.x, position.y,
                                                  CHUNK_SIZE_Z - 1}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    case BLOCK_FACE_FRONT:
        if (position.z < CHUNK_SIZE_Z - 1) {
            return chunk_get_block(chunk, (struct vec3i){position.x, position.y,
                                                         position.z + 1}) ==
                   BLOCK_TYPE_EMPTY;
        } else if (position.z == CHUNK_SIZE_Z - 1 &&
                   neighbors[DIRECTION_FRONT]) {
            return chunk_get_block(neighbors[DIRECTION_FRONT],
                                   (struct vec3i){position.x, position.y, 0}) ==
                   BLOCK_TYPE_EMPTY;
        }
        break;
    }

    return false;
}

// Generate mesh into another memory block then change chunk pointer to this
// one, minimise lock time
void chunk_generate_mesh(struct chunk *chunk, struct chunk **neighbors) {
    chunk->face_count = 0;

    // Probably not efficient
    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < CHUNK_SIZE_X; x++) {
                if (chunk_get_block(chunk, (struct vec3i){x, y, z}) !=
                    BLOCK_TYPE_EMPTY) {
                    for (int i = 0; i < 6; i++) {
                        if (is_block_face_active(chunk, neighbors,
                                                 (struct vec3i){x, y, z}, i)) {
                            chunk->face_count++;
                        }
                    }
                }
            }
        }
    }

    chunk->vertices =
        realloc(chunk->vertices, chunk->face_count * 4 * CHUNK_VERTEX_SIZE *
                                     sizeof(float)); // ADDEDD
    chunk->indices =
        realloc(chunk->indices, chunk->face_count * 6 * sizeof(unsigned int));

    int faces_added = 0;

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < CHUNK_SIZE_X; x++) {
                if (chunk_get_block(chunk, (struct vec3i){x, y, z}) ==
                    BLOCK_TYPE_EMPTY) {
                    continue;
                }

                for (int f = 0; f < 6; f++) {
                    if (!is_block_face_active(chunk, neighbors,
                                              (struct vec3i){x, y, z}, f)) {
                        continue;
                    }

                    for (int i = 0; i < 6; i++) {
                        chunk->indices[faces_added * 6 + i] =
                            INDEX_ORDER[i] + faces_added * 4;
                    }

                    int face_index = (faces_added * 4) * CHUNK_VERTEX_SIZE;

                    for (int i = 0; i < 4; i++) {
                        int vertex_index = face_index + i * CHUNK_VERTEX_SIZE;

                        memcpy(chunk->vertices + vertex_index,
                               VERTEX_POSITIONS[FACE_INDICES[f][i]],
                               3 * sizeof(float));

                        // dont think blocks are positioned correctly as
                        // origin back top left but struct chunk is back bottom
                        // left
                        chunk->vertices[vertex_index] +=
                            x + chunk->position.x * CHUNK_SIZE_X;
                        chunk->vertices[vertex_index + 1] +=
                            y + chunk->position.y * CHUNK_SIZE_Y;
                        chunk->vertices[vertex_index + 2] +=
                            z + chunk->position.z * CHUNK_SIZE_Z;
                    }

                    struct rectangle texture_rectangle =
                        tilemap_get_tile_rectangle(
                            chunk->tilemap,
                            block_type_to_texture(
                                chunk_get_block(chunk, (struct vec3i){x, y, z}))
                                .face_texture_indices[f]);

                    int vertex_1_index = face_index;
                    int vertex_2_index = face_index + CHUNK_VERTEX_SIZE;
                    int vertex_3_index = face_index + 2 * CHUNK_VERTEX_SIZE;
                    int vertex_4_index = face_index + 3 * CHUNK_VERTEX_SIZE;

                    chunk->vertices[vertex_1_index + 3] =
                        texture_rectangle.x; // save index to variable
                    chunk->vertices[vertex_1_index + 4] =
                        texture_rectangle.y + texture_rectangle.height;

                    chunk->vertices[vertex_2_index + 3] =
                        texture_rectangle.x + texture_rectangle.width;
                    chunk->vertices[vertex_2_index + 4] =
                        texture_rectangle.y + texture_rectangle.height;

                    chunk->vertices[vertex_3_index + 3] =
                        texture_rectangle.x + texture_rectangle.width;
                    chunk->vertices[vertex_3_index + 4] = texture_rectangle.y;

                    chunk->vertices[vertex_4_index + 3] = texture_rectangle.x;
                    chunk->vertices[vertex_4_index + 4] = texture_rectangle.y;

                    // Set vertex normals
                    chunk->vertices[vertex_1_index + 5] = FACE_NORMALS[f][0];
                    chunk->vertices[vertex_1_index + 6] = FACE_NORMALS[f][1];
                    chunk->vertices[vertex_1_index + 7] = FACE_NORMALS[f][2];

                    chunk->vertices[vertex_2_index + 5] = FACE_NORMALS[f][0];
                    chunk->vertices[vertex_2_index + 6] = FACE_NORMALS[f][1];
                    chunk->vertices[vertex_2_index + 7] = FACE_NORMALS[f][2];

                    chunk->vertices[vertex_3_index + 5] = FACE_NORMALS[f][0];
                    chunk->vertices[vertex_3_index + 6] = FACE_NORMALS[f][1];
                    chunk->vertices[vertex_3_index + 7] = FACE_NORMALS[f][2];

                    chunk->vertices[vertex_4_index + 5] = FACE_NORMALS[f][0];
                    chunk->vertices[vertex_4_index + 6] = FACE_NORMALS[f][1];
                    chunk->vertices[vertex_4_index + 7] = FACE_NORMALS[f][2];

                    faces_added++;
                }
            }
        }
    }
}

// This isnt very safe
inline enum block_type chunk_get_block(struct chunk *chunk,
                                       struct vec3i position) {
    return chunk->blocks[position.x + position.y * CHUNK_SIZE_X +
                         position.z * CHUNK_SIZE_X * CHUNK_SIZE_Y];
}

inline void chunk_set_block(struct chunk *chunk, struct vec3i position,
                            enum block_type type) {
    chunk->blocks[position.x + position.y * CHUNK_SIZE_X +
                  position.z * (CHUNK_SIZE_X * CHUNK_SIZE_Y)] = type;
    atomic_store(&chunk->mesh_state, CHUNK_MESH_STATE_NEEDED);
}
