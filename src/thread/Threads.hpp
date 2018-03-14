#ifndef SSCE_THREADS_HPP
#define SSCE_THREADS_HPP

#include <Macros.h>
C_DECLS_START
#include "Threads.h"
C_DECLS_END

#include <cstddef>

namespace ssce::thread{
/*
 * Set the name of the calling thread.
 * The string is copied.
 * Returns zero on success.
 */
inline int setName(char* str) {
  return ssce_thread_set_name(str);
}
/*
 * Retrieves the name of the calling
 * thread and stores the result in buffer.
 * Returns zero on success.
 */
inline int getName(char* buffer, size_t len){
  return ssce_thread_get_name(buffer, len);
}
}
#endif /*SSCE_THREADS_HPP*/