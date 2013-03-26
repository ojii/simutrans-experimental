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

#endif
#endif
