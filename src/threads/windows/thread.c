#include "../thread.h"

#include <windows.h>

void thread_create(struct thread *thread, thread_main main, void *arg) {
    thread->handle = CreateThread(NULL, 0, main, arg, 0, NULL);
}

void thread_join(struct thread *thread) {
    WaitForSingleObject(thread->handle, INFINITE);
}

void thread_destroy(struct thread *thread) {
    CloseHandle(thread->handle);
}
