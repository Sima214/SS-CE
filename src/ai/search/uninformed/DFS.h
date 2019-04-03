#ifndef SSCE_SEARCH_DFS_H
#define SSCE_SEARCH_DFS_H
/**
 * @file
 * @brief DFS AI search algorithm.
 */

#include <SearchProblem.h>
#include <Macros.h>

/**
 * Opaque structure containing internal data.
 */
struct DFSState;
typedef struct DFSState DFSState;

/**
 * Allocates a new DFS object.
 * 
 * @returns the allocated object.
 */
EXPORT_API MARK_OBJ_ALLOC DFSState* dfs_create(const ISearchProblem* problem, const void* initial_state) MARK_NONNULL_ARGS(1);

/**
 * Perform a single DFS iteration.
 * 
 * @param dfs \ref dfs_create.
 * @param goal_state where to write the goal state found.
 * @returns 0 if no goal has been reached.
 *   If a goal has been reached, then it writes that state into goal_state and returns 1.
 *   If all the nodes have been searched, then it returns -1.
 *   Any other value indicates a possibly fatal error.
 */
EXPORT_API int dfs_step(DFSState* dfs, void* goal_state) MARK_NONNULL_ARGS(1);

/**
 * Deallocates a previously allocated DFS object.
 * 
 * @param dfs \ref dfs_create.
 */
EXPORT_API void dfs_destroy(DFSState* dfs) MARK_NONNULL_ARGS(1);

#endif /*SSCE_SEARCH_DFS_H*/