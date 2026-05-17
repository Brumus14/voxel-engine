#include "../thread.h"
#include <pthread.h>

void thread_create(struct thread *thread, thread_main main, void *arg) {
    pthread_create(&thread->thread, NULL, main, arg);
}

void thread_cancel(struct thread *thread) {
    pthread_cancel(thread->thread);
}
