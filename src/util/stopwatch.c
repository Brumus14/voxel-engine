#include "stopwatch.h"

#include "platform.h"

#ifdef PLATFORM_POSIX
    #include <time.h>
#elif PLATFORM_WINDOWS
    #include <windows.h>
#endif

double get_time() {
    #ifdef PLATFORM_POSIX
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        return (double)current_time->tv_sec + (double)current_time->tv_nsec / 1e9;
    #elif PLATFORM_WINDOWS
        LARGE_INTEGER counter;
        LARGE_INTEGER frequency;

        QueryPerformanceCounter(&counter);
        QueryPerformanceFrequency(&frequency);

        return (double)counter.QuadPart / (double)frequency.QuadPart;
    #endif
}

void stopwatch_start(struct stopwatch *stopwatch) {
    stopwatch->start_time = get_time();
}

void stopwatch_end(struct stopwatch *stopwatch) {
    stopwatch->end_time = get_time();
}

double stopwatch_elapsed(struct stopwatch *stopwatch) {
    double current_time = get_time();
    return current_time - stopwatch->start_time;
}

double stopwatch_time(struct stopwatch *stopwatch) {
    return stopwatch->end_time - stopwatch->start_time;
}
