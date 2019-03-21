#ifndef SSCE_HEAP_H
#define SSCE_HEAP_H
/**
 * @file
 * @brief Binary heap implementation.
 */

#include <Interface.h>

#include <stddef.h>

/**
 * Used for implementing heapsort.
 * 
 * @param array A pointer to the start of the heap.
 * @param start A pointer to the root element.
 * @param end A pointer to the end of the tree.
 * @param interface A pointer to a IDataType structure
 * defining how interpret array elements.
 */
EXPORT_API void heap_sift_down(void* array, void* start, void* end, const IDataType* interface);

/**
 * Given an array, move the elements around so the array is also a heap.
 * By default it generates a max heap.
 * 
 * @param array A pointer to the start of the array.
 * @param size Element count.
 * @param interface A pointer to a IDataType structure
 * defining how interpret array elements.
 */
EXPORT_API void heap_create(void* array, size_t size, const IDataType* interface);

#endif /*SSCE_HEAP_H*/