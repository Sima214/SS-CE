#ifndef SSCE_HEAP_H
#define SSCE_HEAP_H

#include "Interface.h"
#include <stddef.h>
//TODO: docs
void siftDownHeap(void* array, const DataTypeInterface* interface, size_t start, size_t end);

void createHeap(void* array, size_t size, const DataTypeInterface* interface);

#endif /*SSCE_HEAP_H*/