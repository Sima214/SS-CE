#include "FAlloc.h"

#include <Config.h>
#include <Macros.h>
#include <memory/GAlloc.h>

#include <pthread.h>
#include <stdlib.h>

#if IS_POSIX
  #include <sys/mman.h>
#elif defined(_WIN32)
  #include <windows.h>
  DWORD page_size;
#endif

static pthread_key_t thread_local_stack_key;

static void thread_local_stack_destructor(void* value) {
  ThreadLocalStack* tls = (ThreadLocalStack*)value;
  #if IS_POSIX
    if(COLD_BRANCH(munmap(tls->start, tls->allocated))) {
      // munmap failed!
      EARLY_TRACE("Could not deallocate ThreadLocalStorage stack!");
    }
  #elif defined(_WIN32)
    if(COLD_BRANCH(VirtualFree(tls->start, 0, MEM_RELEASE) == 0)) {
      // Could not deallocate stack!
      EARLY_TRACE("Could not deallocate ThreadLocalStorage stack!");
    }
  #endif
  free(tls);
}

static void init_tls(ThreadLocalStack* tls) {
  #if IS_POSIX
    tls->start = mmap(NULL, MEMORY_FALLOC_STACK_SIZE,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS,
                      -1, 0);
    if(COLD_BRANCH(tls->start == MAP_FAILED)) {
      EARLY_TRACE("Could not allocate thread local stack!");
      abort();
    }
    tls->usage = 0;
    tls->allocated = MEMORY_FALLOC_STACK_SIZE;
  #elif defined(_WIN32)
    // In windows you reserve virtual address space
    // and then commit pages, which reserves ram/swap.
    tls->start = VirtualAlloc(NULL, MEMORY_FALLOC_STACK_SIZE, MEM_RESERVE, PAGE_READWRITE);
    if(COLD_BRANCH(tls->start == NULL)) {
      EARLY_TRACE("Could not reserve thread local stack!");
      abort();
    }
    tls->usage = 0;
    tls->allocated = 0;
    tls->reserved = MEMORY_FALLOC_STACK_SIZE;
  #endif
}

void internal_falloc_init() {
  pthread_key_create(&thread_local_stack_key, thread_local_stack_destructor);
  #if defined(_WIN32)
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    page_size = sys_info.dwPageSize;
  #endif
}

ThreadLocalStack* falloc_get_tls() {
  ThreadLocalStack* tls = pthread_getspecific(thread_local_stack_key);
  if(COLD_BRANCH(tls == NULL)) {
    tls = malloc(sizeof(ThreadLocalStack));
    if(tls == NULL) {
      EARLY_TRACE("Could not allocate memory for thread local stack metadata!");
      abort();
    }
    init_tls(tls);
    pthread_setspecific(thread_local_stack_key, tls);
    EARLY_TRACE("Successfully allocated thread local stack!");
  }
  return tls;
}

int falloc_ensure_space(ThreadLocalStack* tls, size_t l) {
  #if defined(__unix__)
    size_t new_size = tls->usage + l;
    if(COLD_BRANCH(new_size > tls->allocated)) {
      // Try to expand.
      size_t new_allocated = tls->allocated + tls->allocated / 2;
      while(COLD_BRANCH(new_allocated < new_size)) {
        new_allocated += new_allocated / 2;
      }
      void* result = mremap(tls->start, tls->allocated,
                            new_allocated, 0);
      if(result == MAP_FAILED || result != tls->start) {
        EARLY_TRACE("Thread local stack expand failed!");
        return 1;
      }
      else {
        tls->allocated = new_allocated;
        EARLY_TRACE("Successfully expanded thread local stack!");
        return 0;
      }
    }
    else {
      // We have enough space.
      return 0;
    }
  #elif defined(__APPLE__) && defined(__MACH__)
    size_t new_size = tls->usage + l;
    if(COLD_BRANCH(new_size > tls->allocated)) {
      // Macos doesn't have mremap.
      return 1;
    }
    else {
      // We have enough space.
      return 0;
    }
  #elif defined(_WIN32)
    size_t new_size = tls->usage + l;
    // First ensure we have enough virtual address space reserved.
    if(COLD_BRANCH(new_size > tls->reserved)) {
      // Try to expand.
      size_t new_allocated = tls->reserved + tls->reserved / 2;
      while(COLD_BRANCH(new_allocated < new_size)) {
        new_allocated += new_allocated / 2;
      }
      void* result = VirtualAlloc(tls->start, new_allocated, MEM_RESERVE, PAGE_READWRITE);
      if(result == NULL || result != tls->start) {
        EARLY_TRACE("Thread local stack reserved expand failed!");
        return 1;
      }
      else {
        EARLY_TRACE("Successfully expanded reserved thread local stack!");
        tls->reserved = new_allocated;
      }
    }
    // Now ensure enough pages are commited.
    if(COLD_BRANCH(new_size > tls->allocated)) {
      size_t new_commited = tls->allocated + page_size;
      while(COLD_BRANCH(new_commited < new_size)) {
        new_commited += page_size;
      }
      void* result = VirtualAlloc(tls->start, new_commited, MEM_COMMIT, PAGE_READWRITE);
      if(result == NULL || result != tls->start) {
        EARLY_TRACE("Thread local stack commitment expand failed!");
        return 1;
      }
      else {
        EARLY_TRACE("Successfully expanded commited thread local stack!");
        tls->allocated = new_commited;
      }
    }
    // Nothing failed!
    return 0;
  #endif
}

void internal_falloc_exit() {
  // No point in cleaning up here,
  // as long as the destructor above
  // takes care of that for us.
}