#ifndef SSCE_DEQUEUE_H
#define SSCE_DEQUEUE_H
/**
 * @file
 * @brief Double ended linked list of fixed type elements.
 */

#include <Macros.h>
#include <Interface.h>

#include <stddef.h>

/**
 * Opaque structure containing internal data.
 */
struct Dequeue;
typedef struct Dequeue Dequeue;

/**
 * Allocates a new empty Dequeue object.
 * 
 * @param dti A pointer to a \ref IDataType structure.
 * @returns allocated object.
 */
EXPORT_API Dequeue* dequeue_create(const IDataType* dti) MARK_OBJ_ALLOC MARK_NONNULL_ARGS(1);

/**
 * Returns the number of currently stored nodes.
 * 
 * @param dq see \ref dequeue_create.
 * @returns the length of \p dq.
 */
EXPORT_API size_t dequeue_size(Dequeue* dq) MARK_NONNULL_ARGS(1);

/**
 * Inserts a new node at the end of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data pointer to value to be added.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_push_back(Dequeue* dq, const void* data) MARK_NONNULL_ARGS(1, 2);

/**
 * Inserts a new node at the start of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data pointer to value to be added.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_push_front(Dequeue* dq, const void* data) MARK_NONNULL_ARGS(1, 2);

/**
 * Retrieves and removes the node at the end of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data where the node's data will be placed. May be null.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_pop_back(Dequeue* dq, void* data) MARK_NONNULL_ARGS(1);

/**
 * Retrieves and removes the node at the start of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data where the node's data will be placed. May be null.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_pop_front(Dequeue* dq, void* data) MARK_NONNULL_ARGS(1);

/**
 * Retrieves the node at the end of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data where the node's data will be placed.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_peek_back(Dequeue* dq, void* data) MARK_NONNULL_ARGS(1, 2);

/**
 * Retrieves the node at the start of the list.
 * 
 * @param dq see \ref dequeue_create.
 * @param data where the node's data will be placed.
 * @returns non zero on error.
 */
EXPORT_API int dequeue_peek_front(Dequeue* dq, void* data) MARK_NONNULL_ARGS(1, 2);

/**
 * Empties out a dequeue,
 * while freeing allocated memory by the links.
 * 
 * @param dq see \ref dequeue_create.
 */
EXPORT_API void dequeue_reset(Dequeue* dq) MARK_NONNULL_ARGS(1);

/**
 * Destroys a previously created dequeue,
 * and frees all allocated memory.
 * 
 * @param dq see \ref dequeue_create.
 */
EXPORT_API void dequeue_destroy(Dequeue* dq) MARK_NONNULL_ARGS(1);

#endif /*SSCE_DEQUEUE_H*/