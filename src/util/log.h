#ifndef LOG_H
#define LOG_H

#include <pthread.h>
#include <stdio.h>

extern pthread_mutex_t log_lock;

#define WORLD_LOGGING 0
#define GL_LOGGING 0

#if WORLD_LOGGING == 1

#define WORLD_LOG(s)                     \
    if (WORLD_LOGGING) {                 \
        pthread_mutex_lock(&log_lock);   \
        printf("WORLD: ");               \
        s;                               \
        pthread_mutex_unlock(&log_lock); \
    }

#else

#define WORLD_LOG(s)

#endif

#endif
