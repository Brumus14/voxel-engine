#ifndef LOG_H
#define LOG_H

#include "../threads/mutex.h"
#include <stdio.h>

extern struct mutex log_mutex;

#define WORLD_LOGGING 0
#define GL_LOGGING 0

#if WORLD_LOGGING == 1

#define WORLD_LOG(s)                     \
    if (WORLD_LOGGING) {                 \
        mutex_lock(&log_mutex);          \
        printf("WORLD: ");               \
        s;                               \
        mutex_unlock(&log_mutex);        \
    }

#else

#define WORLD_LOG(s)

#endif

void log_init();

#endif
