#include "stopwatch.h"

double timespec_to_seconds(struct timespec *timespec) {
    return (double)timespec->tv_sec + (double)timespec->tv_nsec / 1e9;
}

void get_time(struct timespec *timespec) {
    clock_gettime(CLOCK_MONOTONIC, timespec);
}

void stopwatch_start(struct stopwatch *stopwatch) {
    get_time(&stopwatch->start_time);
}

void stopwatch_end(struct stopwatch *stopwatch) {
    get_time(&stopwatch->end_time);
}

double stopwatch_elapsed(struct stopwatch *stopwatch) {
    struct timespec current_time;
    get_time(&current_time);

    return timespec_to_seconds(&current_time) -
           timespec_to_seconds(&stopwatch->start_time);
}

double stopwatch_time(struct stopwatch *stopwatch) {
    return timespec_to_seconds(&stopwatch->end_time) -
           timespec_to_seconds(&stopwatch->start_time);
}
