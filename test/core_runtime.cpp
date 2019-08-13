#include "test_utils.hpp"

#include <Runtime.hpp>

#include <cstdio>
#include <cstdlib>

int main() {
  ssce::RuntimeInfo rt;
  std::printf("Cpu 64bit addressing:\t%s\n", rt.is64bit() ? "Yes" : "No");
  std::printf("Physical core count:\t%zu\n", rt.getCoreCount());
  std::printf("Threads per core:\t%zu\n", rt.getThreadCountPerCore());
  std::printf("Cache L1i size(bytes):\t%zu\n", rt.getCacheSize(ssce::CPU_CACHE_L1I));
  std::printf("Cache L1d size(bytes):\t%zu\n", rt.getCacheSize(ssce::CPU_CACHE_L1D));
  std::printf("Cache L2 size(bytes):\t%zu\n", rt.getCacheSize(ssce::CPU_CACHE_L2));
  std::printf("Cache L3 size(bytes):\t%zu\n", rt.getCacheSize(ssce::CPU_CACHE_L3));
  std::printf("Cache line alignment:\t%zu\n", rt.getCacheAlignment());
  return EXIT_SUCCESS;
}