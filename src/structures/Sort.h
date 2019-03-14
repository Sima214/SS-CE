#ifndef SSCE_SORT_H
#define SSCE_SORT_H
/**
 * @file
 * @brief A collection of different sort algorithms.
 */

#include <Macros.h>
#include <Interface.h>

#include <stddef.h>

/**
 * Sort an array using the heapsort algorithm.
 * 
 * @param array A pointer to the start of the array.
 * @param size Element count.
 * @param interface A pointer to a IDataType structure
 * defining how interpret array elements.
 */
EXPORT_API void sort_heap(void* array, size_t size, const IDataType* interface);

#endif /*SSCE_SORT_H*/