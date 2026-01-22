#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>

struct stopwatch {
    struct timespec start_time;
    struct timespec end_time;
};

void stopwatch_start(struct stopwatch *stopwatch);
void stopwatch_end(struct stopwatch *stopwatch);
double stopwatch_elapsed(struct stopwatch *stopwatch);
double stopwatch_time(struct stopwatch *stopwatch);

#endif
