#ifndef SSCE_GALLOC_HPP
#define SSCE_GALLOC_HPP
/**
 * @file
 * @brief Overloads new/delete operators with jemalloc ones.
 */

#include <Macros.h>
namespace std {
C_DECLS_START
#include <GAlloc.h>
C_DECLS_END

#include <cstdlib>
#include <new>

void* operator new(std::size_t count) {
  return je_malloc(count);
}

void operator delete(void* ptr) {
  je_free(ptr);
}

#endif /*SSCE_GALLOC_HPP*/