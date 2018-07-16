#ifndef SSCE_SORT_H
#define SSCE_SORT_H
/**
 * @file
 * @brief A collection of different sort algorithms.
 */

#include <stddef.h>

#include <Interface.h>

/**
 * Sort an array using the heapsort algorithm.
 * 
 * @param array A pointer to the start of the array.
 * @param size Element count.
 * @param interface A pointer to a IDataType structure
 * defining how interpret array elements.
 */
void heapsort(void* array, size_t size, const IDataType* interface);

#endif /*SSCE_SORT_H*/