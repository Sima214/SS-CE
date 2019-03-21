#ifndef SSCE_SORT_HPP
#define SSCE_SORT_HPP
/**
 * @file
 * @brief A collection of different sort algorithms.
 */

#include <Macros.h>
C_DECLS_START
#include <Sort.h>
C_DECLS_END

#include <Interface.hpp>

#include <functional>

namespace ssce {

/**
 * Sort an array using the heapsort algorithm.
 * 
 * @param array A pointer to the start of the array.
 * @param size Element count.
 */
template<typename T>
void heapsort(T array[], size_t size) {
  IDataTypeCpp<T> cpptype;
  IDataType type = cpptype.constructIDataType();
  sort_heap(array, size, &type);
}

} // namespace ssce
#endif /*SSCE_SORT_HPP*/