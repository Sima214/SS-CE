#ifndef SSCE_SORTED_ARRAY_H
#define SSCE_SORTED_ARRAY_H
/**
 * @file
 * @brief A self sorting, resizable array.
 */

#include <Interface.h>
#include <Macros.h>

#include <stddef.h>
#include <stdint.h>

/**
 * Opaque structure containing internal data.
 */
struct SortedArray;
typedef struct SortedArray SortedArray;

/**
 * Allocate a new \ref SortedArray object.
 * 
 * @param interface A pointer to a \ref IDataType structure
 * @returns an opaque pointer to the allocated object or null if we are out of memory.
 */
EXPORT_API SortedArray* sorted_array_create(const IDataType* interface) MARK_NONNULL_ARGS(1);

/**
 * Returns the number of elements currently stored in \p sa
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @returns the number of stored elements.
 */
EXPORT_API size_t sorted_array_size(SortedArray* sa) MARK_NONNULL_ARGS(1);

/**
 * Gets a pointer to the internal backing array.
 * The pointer is valid only until the next time \d sa is used.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @returns a pointer to the start of the internal allocated array.
 */
EXPORT_API void* sorted_array_pointer(SortedArray* sa) MARK_NONNULL_ARGS(1);

/**
 * Searches for an index where the specified value is stored.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param value a pointer to the value to search for.
 * @returns a index on the current array where \p value is stored,
 * or \ref INVALID_SIZE_T if the value could not be found.
 */
EXPORT_API size_t sorted_array_find(SortedArray* sa, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Retrieves the stored value at the specified index.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param result pointer to where (e.i. stack) the result will be stored.
 * @param index the index to retrieve.
 * @returns non-zero value if something went wrong.
 * In case of error, the contents of \p result are guaranteed to not have changed.
 */
EXPORT_API int sorted_array_get(SortedArray* sa, void* result, size_t index) MARK_NONNULL_ARGS(1, 2);

/**
 * Inserts a new element, such as the sorted property is maintained.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param value pointer to the value to add.
 * @returns the index where the new element got added,
 * or \ref INVALID_SIZE_T if an error occurred.
 */
EXPORT_API size_t sorted_array_insert(SortedArray* sa, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Inserts a new element, such as the sorted property is maintained.
 * If the element getting added already exists in the array,
 * then the new element is always getting added to the end of the already existing elements.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param value pointer to the value to add.
 * @returns the index where the new element got added,
 * or \ref INVALID_SIZE_T if an error occurred.
 */
EXPORT_API size_t sorted_array_insert_stable(SortedArray* sa, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Merges the contents of \p array into \p sa.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param array pointer to the start of the array of the elements to add.
 * @param count how many elements from \p array to add.
 * @returns non-zero on error. The contents of \p sa may be corrupted in that case.
 */
EXPORT_API int sorted_array_merge(SortedArray* sa, void* array, size_t count) MARK_NONNULL_ARGS(1, 2);

/**
 * Deletes the element at \p index.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param index the index of the element getting removed.
 * @returns non-zero on error.
 * In that case, \p sa is not modified.
 */
EXPORT_API int sorted_array_erase(SortedArray* sa, size_t index) MARK_NONNULL_ARGS(1);

/**
 * Removes an element of \p value.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param value the value of the element to remove.
 * @returns the index of the element which got removed,
 * or \ref INVALID_SIZE_T if an error occurred.
 */
EXPORT_API size_t sorted_array_delete(SortedArray* sa, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Removes the first element of \p value.
 * 
 * @param sa object returned from \ref sorted_array_create.
 * @param value the value of the element to remove.
 * @returns the index of the element which got removed,
 * or \ref INVALID_SIZE_T if an error occurred.
 */
EXPORT_API size_t sorted_array_delete_stable(SortedArray* sa, void* value) MARK_NONNULL_ARGS(1, 2);

/**
 * Resets data structure in the most efficient way.
 * This will delete all elements.
 * 
 * @param sa object returned from \ref sorted_array_create.
 */
EXPORT_API void sorted_array_clear(SortedArray* sa) MARK_NONNULL_ARGS(1);

/**
 * Cleans up and optimizes internal allocated memory.
 * 
 * @param sa object returned from \ref sorted_array_create.
 */
EXPORT_API void sorted_array_compact(SortedArray* sa) MARK_NONNULL_ARGS(1);

/**
 * Deallocates all memory used by this \ref SortedArray.
 * 
 * @param sa object returned from \ref sorted_array_create.
 */
EXPORT_API void sorted_array_destroy(SortedArray* sa) MARK_NONNULL_ARGS(1);

#endif /*SSCE_SORTED_ARRAY_H*/