#ifndef MAT4D_H
#define MAT4D_H

#include "cglm/mat4.h"

#define MAT4D_ZERO                                                        \
    (mat4d) {                                                             \
        {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0}, \
            {0.0, 0.0, 0.0, 0.0},                                         \
    }

struct mat4d {
    double values[4][4];
};

void mat4d_to_glm(struct mat4d *mat, mat4 destination);
struct mat4d mat4d_from_glm(mat4 glm_mat);

#endif
