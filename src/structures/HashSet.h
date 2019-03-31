#ifndef SSCE_HASHSET_H
#define SSCE_HASHSET_H
/**
 * @file
 * @brief Can be used as a HashSet or a HashMap.
 */

#include <Interface.h>
#include <Macros.h>

#include <stddef.h>

/**
 * Opaque structure containing internal data.
 */
struct HashSet;
typedef struct HashSet HashSet;

/**
 * Allocates a new HashSet object.
 * 
 * @param interface \ref interface.
 * @param initial_size How many buckets to allocate at initialization.
 *   This will also be the minimal allocated size.
 *   Use \ref INVALID_SIZE_T or 0 to use the defaults.
 * @param shrink_ratio When the currently stored elements
 *   divided by the current allocated buckets drops below \p shrink_ratio
 *   the number of allocated buckets is reduced.
 *   Pass 0.0 to disable shrinking, or a negative value to use the defaults.
 * @param expand_ratio When the currently stored elements
 *   divided by the current allocated buckets grows above \p expand_ratio
 *   the number of allocated buckets is increased.
 *   Pass 0.0 to disable expanding, or a negative value to use the defaults.
 * @returns the allocated HashSet or NULL if there was not enough memory available.
 */
EXPORT_API HashSet* hashset_create(const IDataType* interface, size_t initial_size, float shrink_ratio, float expand_ratio) MARK_OBJ_ALLOC MARK_NONNULL_ARGS(1);

/**
 * Gets the number of currently stored elements.
 * 
 * @param hs \ref hashset_create.
 * @returns element count.
 */
EXPORT_API size_t hash_size(HashSet* hs) MARK_NONNULL_ARGS(1);

/**
 * Checks if \p value exists in the \ref HashSet.
 * 
 * @param hs \ref hashset_create.
 * @param value pointer to value to search.
 * @returns boolean (0 -> not found).
 */
EXPORT_API int hashset_contains(HashSet* hs, const void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Retrieves the stored element which matches \p value
 * 
 * @param hs \ref hashset_create.
 * @param value pointer to value to retrieve (inout).
 * @returns non zero on error (not found).
 */
EXPORT_API int hashset_get(HashSet* hs, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Adds element \p value if it does not exist in the \ref HashSet.
 * Replaces element \p value if it exists in the \ref HashSet.
 * 
 * @param hs \ref hashset_create.
 * @param value pointer to value to add.
 * @returns non zero on error.
 */
EXPORT_API int hashset_add(HashSet* hs, const void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Removes element \p value if it exists in the \ref HashSet.
 * 
 * @param hs \ref hashset_create.
 * @param value pointer to value to remove.
 * @returns non zero on error.
 */
EXPORT_API int hashset_remove(HashSet* hs, const void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Removes all stored elements in this \ref HashSet.
 * 
 * @param hs \ref hashset_create.
 * @returns non zero on error.
 */
EXPORT_API int hashset_clear(HashSet* hs) MARK_NONNULL_ARGS(1);

/**
 * Deallocates a previously allocated \ref HashSet data structure.
 * 
 * @param hs \ref hashset_create.
 */
EXPORT_API void hashset_destroy(HashSet* hs) MARK_NONNULL_ARGS(1);

// TODO: iterators

#endif /*SSCE_HASHSET_H*/