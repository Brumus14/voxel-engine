#ifndef THREADS_CONDITION_H
#define THREADS_CONDITION_H

#include "../util/platform.h"
#include "mutex.h"

#if PLATFORM_POSIX
#include <pthread.h>

struct condition {
    pthread_cond_t condition;
};

#elif PLATFORM_WINDOWS
#include <windows.h>

struct condition {
    CONDITION_VARIABLE condition;
};
#endif

void condition_init(struct condition *condition);
void condition_wait(struct condition *condition, struct mutex *mutex);
void condition_signal(struct condition *condition);
void condition_broadcast(struct condition *condition);
void condition_destroy(struct condition *condition);

#endif
