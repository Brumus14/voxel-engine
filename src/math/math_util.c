#include "math_util.h"

#include <stdlib.h>
#include <time.h>

bool rand_seeded = false;

struct vec3d rotation_to_direction(struct vec3d rotation) {
    struct vec3d direction;
    rotation.y -= 90;

    direction.x = cos(glm_rad(rotation.y)) * cos(glm_rad(rotation.x));
    direction.y = sin(glm_rad(rotation.x));
    direction.z = sin(glm_rad(rotation.y)) * cos(glm_rad(rotation.x));

    vec3d_normalise(&direction);

    return direction;
}

float clamp(float value, float min, float max) {
    float min_value = value < min ? min : value;
    return min_value > max ? max : min_value;
}

void vec3f_print(struct vec3f v) {
    printf("%f, %f, %f\n", v.x, v.y, v.z);
}

void vec3d_print(struct vec3d v) {
    printf("%f, %f, %f\n", v.x, v.y, v.z);
}

void vec3i_print(struct vec3i v) {
    printf("%d, %d, %d\n", v.x, v.y, v.z);
}

int sign(double value) {
    if (value == 0) {
        return 0;
    }

    return value / fabs(value);
}

int mod(int a, int b) {
    int r = a % b;

    return r < 0 ? r + b : r;
}

double random_range(double min, double max) {
    // maybe separate this for efficiency?
    if (!rand_seeded) {
        srand(time(NULL));

        rand_seeded = true;
    }

    return ((double)rand() / RAND_MAX) * (max - min) + min;
}
