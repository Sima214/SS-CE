#ifndef SSCE_SEARCH_BFS_H
#define SSCE_SEARCH_BFS_H
/**
 * @file
 * @brief BFS AI search algorithm.
 */

#include <SearchProblem.h>
#include <Macros.h>

/**
 * Opaque structure containing internal data.
 */
struct BFSState;
typedef struct BFSState BFSState;

/**
 * Allocates a new BFS object.
 * 
 * @returns the allocated object.
 */
EXPORT_API MARK_OBJ_ALLOC BFSState* bfs_create(const ISearchProblem* problem, const void* initial_state) MARK_NONNULL_ARGS(1);

/**
 * Perform a single BFS iteration.
 * 
 * @param bfs \ref bfs_create.
 * @param goal_state where to write the goal state found.
 * @returns 0 if no goal has been reached.
 *   If a goal has been reached, then it writes that state into goal_state and returns 1.
 *   If all the nodes have been searched, then it returns -1.
 *   Any other value indicates a possibly fatal error.
 */
EXPORT_API int bfs_step(BFSState* bfs, void* goal_state) MARK_NONNULL_ARGS(1);

/**
 * Deallocates a previously allocated BFS object.
 * 
 * @param bfs \ref bfs_create.
 */
EXPORT_API void bfs_destroy(BFSState* bfs) MARK_NONNULL_ARGS(1);

#endif /*SSCE_SEARCH_BFS_H*/