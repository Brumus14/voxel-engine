#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include "../util/platform.h"

#if PLATFORM_POSIX
#include <pthread.h>

typedef void *(*thread_main)(void *);

struct thread {
    pthread_t thread;
};

#elif PLATFORM_WINDOWS
#include <windows.h>

struct mutex {
    HANDLE mutex;
};
#endif

void thread_create(struct thread *thread, thread_main main, void *arg);
void thread_cancel(struct thread *thread);

#endif
