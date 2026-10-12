#include "noise.h"

#include <stdio.h>
#include <stdlib.h>
#include "../math/math_util.h"

fnl_noise_type noise_type_to_fnl(enum noise_type type) {
    switch (type) {
    case NOISE_TYPE_PERLIN:
        return FNL_NOISE_PERLIN;
    case NOISE_TYPE_OPENSIMPLEX2S:
        return FNL_NOISE_OPENSIMPLEX2S;
    }
}

fnl_fractal_type fractal_type_to_fnl(enum noise_fractal_type type) {
    switch (type) {
    case NOISE_FRACTAL_TYPE_NONE:
        return FNL_FRACTAL_NONE;
    case NOISE_FRACTAL_TYPE_FBM:
        return FNL_FRACTAL_FBM;
    }
}

void noise_init(struct noise *noise, enum noise_type noise_type,
                enum noise_fractal_type fractal_type, int seed, float frequency,
                int octaves, float lacunarity, float gain) {
    noise->state = fnlCreateState();
    noise->state.fractal_type = FNL_FRACTAL_FBM;
    noise->state.noise_type = noise_type_to_fnl(noise_type);
    noise->state.fractal_type = fractal_type_to_fnl(fractal_type);
    noise->state.seed = seed;
    noise->state.frequency = frequency;
    noise->state.octaves = octaves;
    noise->state.lacunarity = lacunarity;
    noise->state.gain = gain;
}

float noise_get_2d(struct noise *noise, int x, int y) {
    return fnlGetNoise2D(&noise->state, x, y);
}

struct position_info {
    int min;
    int max;
    float t;
};

float *noise_generate_upscaled_2d(struct noise *noise, int x, int y, int width,
                                  int height, int x_step, int y_step) {
    float *values = malloc(sizeof(float) * width * height);

    int sample_x_min = floor_div(x, x_step);
    int sample_x_max = ceil_div(x + width - 1, x_step);
    int sample_y_min = floor_div(y, y_step);
    int sample_y_max = ceil_div(y + height - 1, y_step);

    int sample_width = sample_x_max - sample_x_min + 1;
    int sample_height = sample_y_max - sample_y_min + 1;

    float *samples = malloc(sizeof(float) * sample_width * sample_height);

    for (int sy = 0; sy < sample_height; sy++) {
        for (int sx = 0; sx < sample_width; sx++) {
            samples[(sy * sample_width) + sx] =
                noise_get_2d(noise, (sample_x_min + sx) * x_step,
                             (sample_y_min + sy) * y_step);
        }
    }

    struct position_info *x_position_infos =
        malloc(sizeof(struct position_info) * width);

    for (int vx = 0; vx < width; vx++) {
        int wx = x + vx;
        x_position_infos[vx].min = floor_div(wx, x_step) - sample_x_min;
        x_position_infos[vx].max = ceil_div(wx, x_step) - sample_x_min;
        x_position_infos[vx].t =
            (float)(wx - floor_div(wx, x_step) * x_step) / x_step;
    }

    for (int vy = 0; vy < height; vy++) {
        int wy = y + vy;
        int sy_min = floor_div(wy, y_step) - sample_y_min;
        int sy_max = ceil_div(wy, y_step) - sample_y_min;
        float ty = (float)(wy - floor_div(wy, y_step) * y_step) / y_step;

        for (int vx = 0; vx < width; vx++) {
            int sx_min = x_position_infos[vx].min;
            int sx_max = x_position_infos[vx].max;
            float tx = x_position_infos[vx].t;

            float sample_top_left = samples[(sy_min * sample_width) + sx_min];
            float sample_top_right = samples[(sy_min * sample_width) + sx_max];
            float sample_bottom_left =
                samples[(sy_max * sample_width) + sx_min];
            float sample_bottom_right =
                samples[(sy_max * sample_width) + sx_max];

            float top =
                sample_top_left + tx * (sample_top_right - sample_top_left);
            float bottom = sample_bottom_left +
                           tx * (sample_bottom_right - sample_bottom_left);

            values[(vy * width) + vx] = top + ty * (bottom - top);
        }
    }

    free(x_position_infos);
    free(samples);

    return values;
}

float noise_get_3d(struct noise *noise, int x, int y, int z) {
    return fnlGetNoise3D(&noise->state, x, y, z);
}

float *noise_generate_upscaled_3d(struct noise *noise, int x, int y, int z,
                                  int width, int height, int depth, int x_step,
                                  int y_step, int z_step) {
    float *values = malloc(sizeof(float) * width * height * depth);

    int sample_x_min = floor_div(x, x_step);
    int sample_x_max = ceil_div(x + width - 1, x_step);
    int sample_y_min = floor_div(y, y_step);
    int sample_y_max = ceil_div(y + height - 1, y_step);
    int sample_z_min = floor_div(z, z_step);
    int sample_z_max = ceil_div(z + depth - 1, z_step);

    int sample_width = sample_x_max - sample_x_min + 1;
    int sample_height = sample_y_max - sample_y_min + 1;
    int sample_depth = sample_z_max - sample_z_min + 1;

    float *samples =
        malloc(sizeof(float) * sample_width * sample_height * sample_depth);

    for (int sz = 0; sz < sample_depth; sz++) {
        for (int sy = 0; sy < sample_height; sy++) {
            for (int sx = 0; sx < sample_width; sx++) {
                samples[((sz * sample_height + sy) * sample_width) + sx] =
                    noise_get_3d(noise, (sample_x_min + sx) * x_step,
                                 (sample_y_min + sy) * y_step,
                                 (sample_z_min + sz) * z_step);
            }
        }
    }

    struct position_info *y_position_infos =
        malloc(sizeof(struct position_info) * height);

    struct position_info *x_position_infos =
        malloc(sizeof(struct position_info) * width);

    for (int vx = 0; vx < width; vx++) {
        int wx = x + vx;
        x_position_infos[vx].min = floor_div(wx, x_step) - sample_x_min;
        x_position_infos[vx].max = ceil_div(wx, x_step) - sample_x_min;
        x_position_infos[vx].t =
            (float)(wx - floor_div(wx, x_step) * x_step) / x_step;
    }

    for (int vz = 0; vz < depth; vz++) {
        int wz = z + vz;
        int sz_min = floor_div(wz, z_step) - sample_z_min;
        int sz_max = ceil_div(wz, z_step) - sample_z_min;
        float tz = (float)(wz - floor_div(wz, z_step) * z_step) / z_step;

        for (int vy = 0; vy < height; vy++) {
            int wy = y + vy;
            y_position_infos[vy].min = floor_div(wy, y_step) - sample_y_min;
            y_position_infos[vy].max = ceil_div(wy, y_step) - sample_y_min;
            y_position_infos[vy].t =
                (float)(wy - floor_div(wy, y_step) * y_step) / y_step;
        }

        for (int vy = 0; vy < height; vy++) {
            int sy_min = y_position_infos[vy].min;
            int sy_max = y_position_infos[vy].max;
            float ty = y_position_infos[vy].t;

            for (int vx = 0; vx < width; vx++) {
                int sx_min = x_position_infos[vx].min;
                int sx_max = x_position_infos[vx].max;
                float tx = x_position_infos[vx].t;

                float sample_back_top_left =
                    samples[((sz_min * sample_height + sy_min) * sample_width) +
                            sx_min];
                float sample_back_top_right =
                    samples[((sz_min * sample_height + sy_min) * sample_width) +
                            sx_max];
                float sample_back_bottom_left =
                    samples[((sz_min * sample_height + sy_max) * sample_width) +
                            sx_min];
                float sample_back_bottom_right =
                    samples[((sz_min * sample_height + sy_max) * sample_width) +
                            sx_max];
                float sample_front_top_left =
                    samples[((sz_max * sample_height + sy_min) * sample_width) +
                            sx_min];
                float sample_front_top_right =
                    samples[((sz_max * sample_height + sy_min) * sample_width) +
                            sx_max];
                float sample_front_bottom_left =
                    samples[((sz_max * sample_height + sy_max) * sample_width) +
                            sx_min];
                float sample_front_bottom_right =
                    samples[((sz_max * sample_height + sy_max) * sample_width) +
                            sx_max];

                float back_top =
                    sample_back_top_left +
                    tx * (sample_back_top_right - sample_back_top_left);
                float back_bottom =
                    sample_back_bottom_left +
                    tx * (sample_back_bottom_right - sample_back_bottom_left);
                float back = back_top + ty * (back_bottom - back_top);

                float front_top =
                    sample_front_top_left +
                    tx * (sample_front_top_right - sample_front_top_left);
                float front_bottom =
                    sample_front_bottom_left +
                    tx * (sample_front_bottom_right - sample_front_bottom_left);
                float front = front_top + ty * (front_bottom - front_top);

                values[((vz * height + vy) * width) + vx] =
                    back + tz * (front - back);
            }
        }
    }

    free(x_position_infos);
    free(y_position_infos);
    free(samples);

    return values;
}
