#include "Runtime.h"

#include <stddef.h>
#include <sys/sysctl.h>
#include <unistd.h>

void internal_runtime_init_os(Runtime* rt) {
  rt->cpu_page_size = sysconf(_SC_PAGESIZE);
  size_t size_t_size = sizeof(size_t);
  sysctlbyname("hw.physicalcpu", &rt->cpu_core_count, &size_t_size, NULL, 0);
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.logicalcpu", &rt->cpu_threads_per_core, &size_t_size, NULL, 0);
  rt->cpu_threads_per_core = rt->cpu_threads_per_core / rt->cpu_core_count;
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.l1icachesize", &rt->cpu_cache_size_l1i, &size_t_size, NULL, 0);
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.l1dcachesize", &rt->cpu_cache_size_l1d, &size_t_size, NULL, 0);
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.l2cachesize", &rt->cpu_cache_size_l2, &size_t_size, NULL, 0);
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.l3cachesize", &rt->cpu_cache_size_l3, &size_t_size, NULL, 0);
  size_t_size = sizeof(size_t);
  sysctlbyname("hw.cachelinesize", &rt->cpu_cache_alignment, &size_t_size, NULL, 0);
}