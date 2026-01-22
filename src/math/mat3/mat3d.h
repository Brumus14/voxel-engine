#ifndef MAT3D_H
#define MAT3D_H

#include "cglm/mat3.h"

#define MAT3D_ZERO                                         \
    (mat3d) {                                              \
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, \
    }

struct mat3d {
    double values[3][3];
};

void mat3d_to_glm(struct mat3d *mat, mat3 destination);
struct mat3d mat3d_from_glm(mat3 glm_mat);

#endif
