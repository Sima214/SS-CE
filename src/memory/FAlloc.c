#include "FAlloc.h"

#include <Macros.h>
#include <memory/GAlloc.h>

#include <pthread.h>

#if IS_POSIX
  #include <sys/mman.h>
#elif defined(_WIN32)
  #include <memoryapi.h>
#endif


static pthread_key_t thread_local_stack_key;

static void thread_local_stack_destructor(void* value) {
  ThreadLocalStack* tls = (ThreadLocalStack*)value;
  #if IS_POSIX
  #elif defined(_WIN32)
  #endif
  free(tls);
}

void internal_falloc_init() {
  pthread_key_create(&thread_local_stack_key, thread_local_stack_destructor);
}

ThreadLocalStack* get_local_thread_stack() {
  #if IS_POSIX
  #elif defined(_WIN32)
  #endif
  return NULL;
}

int ensure_allocated_space(ThreadLocalStack* tls, size_t l) {
  #if IS_POSIX
  #elif defined(_WIN32)
  #endif
  return 0;
}

void internal_falloc_exit() {
  // No point in cleaning up here,
  // as long as the destructor above
  // takes care of that for us.
}
