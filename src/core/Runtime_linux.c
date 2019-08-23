#include "Runtime.h"

#include <math/MinMax.h>

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define SYS_CPU0_BASE(name) "/sys/devices/system/cpu/cpu0/topology/" name

static inline uint8_t hex_bit_count(int c) {
  c = toupper(c);
  // Creates a 21 byte lookup table.
  switch(c) {
    case '0':
      return 0;
    case '1':
    case '2':
    case '4':
    case '8':
      return 1;
    case '3':
    case '5':
    case '6':
    case '9':
    case 'A':
    case 'C':
      return 2;
    case '7':
    case 'B':
    case 'D':
    case 'E':
      return 3;
    case 'F':
      return 4;
    default:
      EARLY_TRACEF("Skipping character `#%u` from `" SYS_CPU0_BASE("thread_siblings") "`...", (char)c);
      return 0;
  }
}

static inline size_t internal_get_threads_per_core() {
  FILE* f = fopen(SYS_CPU0_BASE("thread_siblings"), "r");
  if(COLD_BRANCH(f == NULL)) {
    EARLY_TRACE("Could not find `" SYS_CPU0_BASE("thread_siblings") "`!");
    return 1;
  }
  size_t sibling_count = 0;
  int cur;
  while((cur = fgetc(f)) != EOF) {
    sibling_count += hex_bit_count(cur);
  }
  fclose(f);
  return math_max(1U, sibling_count);
}

void internal_runtime_init_os(Runtime* rt) {
  rt->cpu_page_size = sysconf(_SC_PAGESIZE);
  // Count the number of cpu threads cpu0 shares with other cpus.
  rt->cpu_threads_per_core = internal_get_threads_per_core();
  rt->cpu_core_count = sysconf(_SC_NPROCESSORS_ONLN) / rt->cpu_threads_per_core;
  // Cache info, similar to mac's just without pointers.
  rt->cpu_cache_size_l1i = sysconf(_SC_LEVEL1_ICACHE_SIZE);
  rt->cpu_cache_size_l1d = sysconf(_SC_LEVEL1_DCACHE_SIZE);
  rt->cpu_cache_size_l2 = sysconf(_SC_LEVEL2_CACHE_SIZE);
  rt->cpu_cache_size_l3 = sysconf(_SC_LEVEL3_CACHE_SIZE);
  // Just to be sure...
  rt->cpu_cache_alignment = sysconf(_SC_LEVEL1_ICACHE_LINESIZE);
  rt->cpu_cache_alignment = math_max(rt->cpu_cache_alignment, (size_t)sysconf(_SC_LEVEL1_DCACHE_LINESIZE));
  rt->cpu_cache_alignment = math_max(rt->cpu_cache_alignment, (size_t)sysconf(_SC_LEVEL2_CACHE_LINESIZE));
  rt->cpu_cache_alignment = math_max(rt->cpu_cache_alignment, (size_t)sysconf(_SC_LEVEL3_CACHE_LINESIZE));
}