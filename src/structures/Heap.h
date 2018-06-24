#ifndef SSCE_HEAP_H
#define SSCE_HEAP_H

#include "Interface.h"
#include <stddef.h>

/*
 * Usefull when sorting.
 */
void siftDownHeap(void* array, const DataTypeInterface* interface, size_t start, size_t end);

/*
 * Given an array, move the elements
 * around so the array is also a heap.
 * By default it generates a max heap.
 */
void createHeap(void* array, size_t size, const DataTypeInterface* interface);

#endif /*SSCE_HEAP_H*/