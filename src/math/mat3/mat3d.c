#include "mat3d.h"

void mat3d_to_glm(struct mat3d *mat, mat3 destination) {
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            destination[y][x] = mat->values[y][x];
        }
    }
}

struct mat3d mat3d_from_glm(mat3 glm_mat) {
    struct mat3d mat;

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            mat.values[y][x] = glm_mat[y][x];
        }
    }

    return mat;
}
