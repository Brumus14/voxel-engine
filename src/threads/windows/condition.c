#include "../condition.h"
#include <pthread.h>

// void condition_init(struct condition *condition) {
//     pthread_cond_init(&condition->condition, NULL);
// }
//
// void condition_wait(struct condition *condition, struct mutex *mutex) {
//     pthread_cond_wait(&condition->condition, &mutex->mutex);
// }
//
// void condition_signal(struct condition *condition) {
//     pthread_cond_signal(&condition->condition);
// }
//
// void condition_destroy(struct condition *condition) {
//     pthread_cond_destroy(&condition->condition);
// }
