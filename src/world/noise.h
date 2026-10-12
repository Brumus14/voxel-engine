#ifndef NOISE_H
#define NOISE_H

#include "FastNoiseLite.h"

enum noise_type {
    NOISE_TYPE_PERLIN,
    NOISE_TYPE_OPENSIMPLEX2S,
};

enum noise_fractal_type {
    NOISE_FRACTAL_TYPE_NONE,
    NOISE_FRACTAL_TYPE_FBM,
};

struct noise {
    fnl_state state;
};

void noise_init(struct noise *noise, enum noise_type noise_type,
                enum noise_fractal_type fractal_type, int seed, float frequency,
                int octaves, float lacunarity, float gain);

float noise_get_2d(struct noise *noise, int x, int y);
float *noise_generate_upscaled_2d(struct noise *noise, int x, int y, int width,
                                  int height, int x_step, int y_step);

float noise_get_3d(struct noise *noise, int x, int y, int z);
// TODO: Switch from individual components to vec3i
float *noise_generate_upscaled_3d(struct noise *noise, int x, int y, int z,
                                  int width, int height, int depth, int x_step,
                                  int y_step, int z_step);

#endif
