#ifndef SSCE_POSIXTHREADS
#define SSCE_POSIXTHREADS
/**
* @file
* @brief Cross platform interface with pthreads with some extras.
*/

#include <Macros.h>

#include <pthread.h>

#ifdef __APPLE__
    #include <os/lock.h>
    
    typedef os_unfair_lock pthread_spinlock_t;

    EXPORT_API int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
    EXPORT_API int pthread_spin_lock(pthread_spinlock_t *lock);
    EXPORT_API int pthread_spin_trylock(pthread_spinlock_t *lock);
    EXPORT_API int pthread_spin_unlock(pthread_spinlock_t *lock);
    EXPORT_API int pthread_spin_destroy(pthread_spinlock_t *lock);
#endif

#endif /*SSCE_POSIXTHREADS*/