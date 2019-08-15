#include "Runtime.h"

#include <Macros.h>
#include <math/MinMax.h>
#include <memory/FAlloc.h>

#include <stddef.h>
#include <stdint.h>
#include <windows.h>

/**
 * The four sizes of 'windows' cache.
 */
typedef struct {
  DWORD unified;
  DWORD instruction;
  DWORD data;
  DWORD trace;
} WindowsCacheSizes;

#define add_offset(pointer, offset) (__typeof__(pointer))(((uintptr_t)pointer) + ((size_t)offset))

#define register_cache(cache_info, cache_sizes)                                             \
  switch(cache_info->Type) {                                                                \
    case CacheUnified:                                                                      \
      if(cache_sizes.unified == 0) {                                                        \
        cache_sizes.unified = cache_info->CacheSize;                                        \
      } else {                                                                              \
        cache_sizes.unified = math_min(cache_sizes.unified, cache_info->CacheSize);         \
      }                                                                                     \
      break;                                                                                \
    case CacheInstruction:                                                                  \
      if(cache_sizes.instruction == 0) {                                                    \
        cache_sizes.instruction = cache_info->CacheSize;                                    \
      } else {                                                                              \
        cache_sizes.instruction = math_min(cache_sizes.instruction, cache_info->CacheSize); \
      }                                                                                     \
      break;                                                                                \
    case CacheData:                                                                         \
      if(cache_sizes.data == 0) {                                                           \
        cache_sizes.data = cache_info->CacheSize;                                           \
      } else {                                                                              \
        cache_sizes.data = math_min(cache_sizes.data, cache_info->CacheSize);               \
      }                                                                                     \
      break;                                                                                \
    case CacheTrace:                                                                        \
      if(cache_sizes.trace == 0) {                                                          \
        cache_sizes.trace = cache_info->CacheSize;                                          \
      } else {                                                                              \
        cache_sizes.trace = math_min(cache_sizes.trace, cache_info->CacheSize);             \
      }                                                                                     \
      break;                                                                                \
    default:                                                                                \
      EARLY_TRACEF("Unknown cache type: %i", cache_info->Type);                             \
      break;                                                                                \
  }

static void internal_runtime_init_os_cpu(Runtime* rt) {
  // Cpu information.
  DWORD cpu_info_size = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* cpu_info;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* cpu_info_end;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX* cpu_info_current;
  // Request info using new api - required buffer calculation.
  BOOL ret_00 = GetLogicalProcessorInformationEx(RelationAll, NULL, &cpu_info_size);
  if(COLD_BRANCH(ret_00 || GetLastError() != ERROR_INSUFFICIENT_BUFFER)) {
    EARLY_TRACE("Could not calculate required buffer size for CPU info!");
    return;
  } else {
    EARLY_TRACEF("Allocating %lu bytes for cpu_info!", cpu_info_size);
  }
  // Allocate temp buffer.
  cpu_info = falloc_malloc(cpu_info_size);
  if(COLD_BRANCH(cpu_info == NULL)) {
    EARLY_TRACE("Could not allocate the required buffer for CPU info!");
    return;
  }
  // Actual request of cpu information.
  BOOL ret_01 = GetLogicalProcessorInformationEx(RelationAll, cpu_info, &cpu_info_size);
  if(COLD_BRANCH(!ret_01)) {
    EARLY_TRACEF("Could not obtain cpu info from GetLogicalProcessorInformationEx(RelationAll, %p, %lu)->0x%08lx!", cpu_info, cpu_info_size, GetLastError());
    return;
  }
  cpu_info_end = add_offset(cpu_info, cpu_info_size);
  // Parse structures returned from api.
  size_t total_cores = 0;
  size_t total_threads = 0;
  WindowsCacheSizes l1 = {0, 0, 0, 0};
  WindowsCacheSizes l2 = {0, 0, 0, 0};
  WindowsCacheSizes l3 = {0, 0, 0, 0};
  for(cpu_info_current = cpu_info; cpu_info_current != cpu_info_end; cpu_info_current = add_offset(cpu_info_current, cpu_info_current->Size)) {
    switch(cpu_info_current->Relationship) {
      case RelationProcessorCore: {
        // PROCESSOR_RELATIONSHIP* core = &cpu_info_current->Processor;
        total_cores++;
        break;
      }
      case RelationNumaNode: {
        // NUMA_NODE_RELATIONSHIP* numa = &cpu_info_current->NumaNode;
        break;
      }
      case RelationCache: {
        CACHE_RELATIONSHIP* cache = &cpu_info_current->Cache;
        switch(cache->Level) {
          case 1:
            register_cache(cache, l1);
            break;
          case 2:
            register_cache(cache, l2);
            break;
          case 3:
            register_cache(cache, l3);
            break;
          default:
            EARLY_TRACEF("Unhandled L%i cache!", cache->Level);
            break;
        }
        rt->cpu_cache_alignment = math_max(rt->cpu_cache_alignment, cache->LineSize);
        break;
      }
      case RelationProcessorPackage: {
        // PROCESSOR_RELATIONSHIP* pkg = &cpu_info_current->Processor;
        break;
      }
      case RelationGroup: {
        GROUP_RELATIONSHIP* group = &cpu_info_current->Group;
        for(int i = 0; i < group->ActiveGroupCount; i++) {
          total_threads += group->GroupInfo[i].ActiveProcessorCount;
        }
        break;
      }
      default:
        EARLY_TRACE("Unrecognized CPU info structure!");
        break;
    }
  }
  falloc_free(cpu_info);
  // Finish up.
  rt->cpu_core_count = total_cores;
  rt->cpu_threads_per_core = total_threads / total_cores;
  rt->cpu_cache_size_l1i = l1.instruction;
  rt->cpu_cache_size_l1d = l1.data + l1.unified;
  rt->cpu_cache_size_l2 = l2.unified + l2.data + l2.instruction;
  rt->cpu_cache_size_l3 = l3.unified + l3.data + l3.instruction;
}

void internal_runtime_init_os(Runtime* rt) {
  // Page size.
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  rt->cpu_page_size = si.dwPageSize;
  internal_runtime_init_os_cpu(rt);
}