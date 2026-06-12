#include "world_generation.h"

#include "FastNoiseLite.h"
#include "chunk.h"

static inline int block_index(int x, int y, int z) {
    return z * CHUNK_SIZE_Y * CHUNK_SIZE_X + y * CHUNK_SIZE_X + x;
}

void upscale_noise(double *output[CHUNK_SIZE_Z][CHUNK_SIZE_X], fnl_state noise,
                   unsigned int level) {
    unsigned int step = pow(level, 2);

    malloc(CHUNK_SIZE_Z / level * CHUNK_SIZE_X / level * sizeof(double));

    for (int y = 0; y < CHUNK_SIZE_Z; y += step) {
        fnlGetNoise2D(noise, );
    }
}

// TODO: This is very messy
_Atomic(enum block_type) *
world_generation_chunk_terrain(struct vec3i chunk_position, int seed) {
    _Atomic(enum block_type) *blocks =
        malloc(sizeof(_Atomic(enum block_type)) * CHUNK_SIZE_X * CHUNK_SIZE_Y *
               CHUNK_SIZE_Z);

    fnl_state height_noise = fnlCreateState();
    height_noise.noise_type = FNL_NOISE_PERLIN;
    height_noise.seed = seed;
    height_noise.frequency = 0.1;
    height_noise.octaves = 10;
    height_noise.lacunarity = 8;
    height_noise.gain = 0.8;

    struct vec3i chunk_block_position;
    vec3i_init(&chunk_block_position, chunk_position.x * CHUNK_SIZE_X,
               chunk_position.y * CHUNK_SIZE_Y,
               chunk_position.z * CHUNK_SIZE_Z);

    for (int z = 0; z < CHUNK_SIZE_Z; z++) {
        int position_z = chunk_block_position.z + z;

        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            int position_x = chunk_block_position.x + x;

            float height_value =
                fnlGetNoise2D(&height_noise, position_x, position_z) * 16;

            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                int position_y = chunk_block_position.y + y;

                enum block_type type = BLOCK_TYPE_EMPTY;

                if (position_y < height_value - 8) {
                    type = BLOCK_TYPE_STONE;
                } else if (position_y < height_value) {
                    type = BLOCK_TYPE_DIRT;
                } else if (position_y == ceil(height_value)) {
                    type = BLOCK_TYPE_GRASS;
                }

                blocks[block_index(x, y, z)] = type;
            }
        }
    }

    // // better ore generation
    // // use octaves and make noise generator abstraction
    // fnl_state cave_noise = fnlCreateState();
    // cave_noise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    // cave_noise.seed = seed;
    //
    // fnl_state coal_noise = fnlCreateState();
    // coal_noise.noise_type = FNL_NOISE_PERLIN;
    // coal_noise.seed = seed;
    //
    // fnl_state diamond_noise = fnlCreateState();
    // diamond_noise.noise_type = FNL_NOISE_OPENSIMPLEX2S;
    // diamond_noise.seed = seed + 1;
    //
    // for (int z = 0; z < CHUNK_SIZE_Z; z++) {
    //     for (int y = 0; y < CHUNK_SIZE_Y; y++) {
    //         for (int x = 0; x < CHUNK_SIZE_X; x++) {
    //             struct vec3i position = {chunk_block_position.x + x,
    //                                      chunk_block_position.y + y,
    //                                      chunk_block_position.z + z};
    //
    //             if (fnlGetNoise3D(&cave_noise, position.x * 2, position.y *
    //             2,
    //                               position.z * 2) > 0.3) {
    //                 blocks[block_index(x, y, z)] = BLOCK_TYPE_EMPTY;
    //             } else {
    //                 if (blocks[block_index(x, y, z)] == BLOCK_TYPE_STONE) {
    //                     if (fnlGetNoise3D(&cave_noise, position.x * 2,
    //                                       position.y * 2,
    //                                       position.z * 2) > 0.3) {
    //                         blocks[block_index(x, y, z)] = BLOCK_TYPE_EMPTY;
    //                     } else if (fnlGetNoise3D(&coal_noise, position.x *
    //                     20,
    //                                              position.y * 20,
    //                                              position.z * 20) > 0.6) {
    //                         blocks[block_index(x, y, z)] = BLOCK_TYPE_COAL;
    //                     } else if (fnlGetNoise3D(&diamond_noise,
    //                                              position.x * 20,
    //                                              position.y * 20,
    //                                              position.z * 20) > 0.91) {
    //                         blocks[block_index(x, y, z)] =
    //                         BLOCK_TYPE_DIAMOND;
    //                     }
    //                 } else if (blocks[block_index(x, y, z)] ==
    //                            BLOCK_TYPE_DIRT) {
    //                     if (fnlGetNoise3D(&cave_noise, position.x * 2,
    //                                       (position.y + 1) * 2,
    //                                       position.z * 2) > 0.3) {
    //                         blocks[block_index(x, y, z)] = BLOCK_TYPE_GRASS;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    return blocks;
}
