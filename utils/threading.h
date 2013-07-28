#ifndef threading_h
#define threading_h
#if MULTI_THREAD>1

// enable barriers by this
#define _XOPEN_SOURCE 600
#include <pthread.h>

// Mac OS X defines this initializers without _NP.
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#endif

// add pthread barrier support for OSX
typedef struct {
    int needed;
    int called;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} pthread_barrier_t;

// needed because our signature doesn't match the one from pthread_barrier_init(3)
#define pthread_barrier_init(b,a,n) barrier_init(b,n)
int barrier_init(pthread_barrier_t *barrier, int needed);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_wait(pthread_barrier_t *barrier); 

#endif
#endif
