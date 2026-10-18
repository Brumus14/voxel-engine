#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include "../util/platform.h"

#if PLATFORM_POSIX
#include <pthread.h>

struct thread {
    pthread_t thread;
};

#elif PLATFORM_WINDOWS
#include <windows.h>

struct thread {
    HANDLE handle;
};
#endif

typedef void *(*thread_main)(void *);

void thread_create(struct thread *thread, thread_main main, void *arg);
void thread_join(struct thread *thread);
void thread_destroy(struct thread *thread);

#endif
