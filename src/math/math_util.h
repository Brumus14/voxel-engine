#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "vec3/vec3f.h"
#include "vec3/vec3d.h"
#include "vec3/vec3i.h"

struct vec3d rotation_to_direction(struct vec3d rotation);
float clamp(float value, float min, float max);
void vec3f_print(struct vec3f v);
void vec3d_print(struct vec3d v);
void vec3i_print(struct vec3i v);
int sign(double value);
int mod(int a, int b);
double random_range(double min, double max);

static inline double min(double a, double b) {
    return (a < b) ? a : b;
}

static inline double max(double a, double b) {
    return (a > b) ? a : b;
}

static inline int floor_div(int n, int d) {
    int q = n / d;
    int r = n % d;

    if ((r > 0 && d < 0) || (r < 0 && d > 0)) {
        q--;
        r += d;
    }

    return q;
}

#endif
