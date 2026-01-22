#include "mat4d.h"

void mat4d_to_glm(struct mat4d *mat, mat4 destination) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            destination[y][x] = mat->values[y][x];
        }
    }
}

struct mat4d mat4d_from_glm(mat4 glm_mat) {
    struct mat4d mat;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            mat.values[y][x] = glm_mat[y][x];
        }
    }

    return mat;
}
