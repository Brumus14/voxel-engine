#include "world_generation.h"

#include "FastNoiseLite.h"
#include "chunk.h"

static inline int block_index(int x, int y, int z) {
    return z * CHUNK_SIZE_Y * CHUNK_SIZE_X + y * CHUNK_SIZE_X + x;
}

enum block_type *world_generation_chunk_terrain(struct vec3i chunk_position,
                                                float seed) {
    enum block_type *blocks = malloc(sizeof(enum block_type) * CHUNK_SIZE_X *
                                     CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    fnl_state height_noise = fnlCreateState();
    height_noise.noise_type = FNL_NOISE_PERLIN;
    height_noise.seed = seed;

    struct vec3i chunk_block_position;
    vec3i_init(&chunk_block_position, chunk_position.x * CHUNK_SIZE_X,
               chunk_position.y * CHUNK_SIZE_Y,
               chunk_position.z * CHUNK_SIZE_Z);

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < CHUNK_SIZE_X; x++) {
                struct vec3i position;
                vec3i_init(&position, chunk_block_position.x + x,
                           chunk_block_position.y + y,
                           chunk_block_position.z + z);

                enum block_type type = BLOCK_TYPE_EMPTY;

                float height_value =
                    fnlGetNoise2D(&height_noise, position.x * 2,
                                  position.z * 2) *
                    16;

                if (position.y < height_value - 8) {
                    type = BLOCK_TYPE_STONE;
                } else if (position.y < height_value) {
                    type = BLOCK_TYPE_DIRT;
                } else if (position.y == ceil(height_value)) {
                    type = BLOCK_TYPE_GRASS;
                }

                blocks[block_index(x, y, z)] = type;
            }
        }
    }

    // better ore generation
    // use octaves and make noise generator abstraction
    fnl_state cave_noise = fnlCreateState();
    cave_noise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    cave_noise.seed = seed;

    fnl_state coal_noise = fnlCreateState();
    coal_noise.noise_type = FNL_NOISE_PERLIN;
    coal_noise.seed = seed;

    fnl_state diamond_noise = fnlCreateState();
    diamond_noise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    diamond_noise.seed = seed + 1;

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < CHUNK_SIZE_X; x++) {
                struct vec3i position = {chunk_block_position.x + x,
                                         chunk_block_position.y + y,
                                         chunk_block_position.z + z};

                if (fnlGetNoise3D(&cave_noise, position.x * 2, position.y * 2,
                                  position.z * 2) > 0.3) {
                    blocks[block_index(x, y, z)] = BLOCK_TYPE_EMPTY;
                } else {
                    if (blocks[block_index(x, y, z)] == BLOCK_TYPE_STONE) {
                        if (fnlGetNoise3D(&cave_noise, position.x * 2,
                                          position.y * 2,
                                          position.z * 2) > 0.3) {
                            blocks[block_index(x, y, z)] = BLOCK_TYPE_EMPTY;
                        } else if (fnlGetNoise3D(&coal_noise, position.x * 20,
                                                 position.y * 20,
                                                 position.z * 20) > 0.6) {
                            blocks[block_index(x, y, z)] = BLOCK_TYPE_COAL;
                        } else if (fnlGetNoise3D(&diamond_noise,
                                                 position.x * 20,
                                                 position.y * 20,
                                                 position.z * 20) > 0.91) {
                            blocks[block_index(x, y, z)] = BLOCK_TYPE_DIAMOND;
                        }
                    } else if (blocks[block_index(x, y, z)] ==
                               BLOCK_TYPE_DIRT) {
                        if (fnlGetNoise3D(&cave_noise, position.x * 2,
                                          (position.y + 1) * 2,
                                          position.z * 2) > 0.3) {
                            blocks[block_index(x, y, z)] = BLOCK_TYPE_GRASS;
                        }
                    }
                }
            }
        }
    }

    return blocks;
}
