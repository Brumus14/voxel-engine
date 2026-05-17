#ifndef THREADS_MUTEX_H
#define THREADS_MUTEX_H

#include "../util/platform.h"

#if PLATFORM_POSIX
#include <pthread.h>

struct mutex {
    pthread_mutex_t mutex;
};

#elif PLATFORM_WINDOWS
#include <windows.h>

struct mutex {};
#endif

void mutex_init(struct mutex *mutex);
void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);
void mutex_destroy(struct mutex *mutex);

#endif
