#include "../thread.h"

void thread_create(struct thread *thread, thread_main main, void *arg) {
    pthread_create(&thread->thread, NULL, main, arg);
}

void thread_join(struct thread *thread) {
    pthread_join(thread->thread, NULL);
}

void thread_destroy(struct thread *thread) {
}
