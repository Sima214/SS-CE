#ifndef SSCE_THREADS_H
#define SSCE_THREADS_H

#include <Macros.h>

/*
 * OS which support the POSIX api.
 */
#if IS_POSIX

#include <pthread.h>

/*
 * Set the name of the calling thread.
 * The string is copied.
 * Returns zero on success.
 */
#define ssce_thread_set_name(str) pthread_setname_np(pthread_self(), str)
/*
 * Retrieves the name of the calling
 * thread and stores the result in buffer.
 * Returns zero on success.
 */
#define ssce_thread_get_name(buffer, len) pthread_getname_np(pthread_self(), buffer, len)

#endif
/*
 * Code for the Win32 api.
 */
#ifdef _WIN32


#endif
#endif /*SSCE_THREADS_H*/