#ifndef SSCE_SEARCH_BESTFIRST_H
#define SSCE_SEARCH_BESTFIRST_H
/**
 * @file
 * @brief Best First Search algorithm.
 */

#include <SearchProblem.h>
#include <Macros.h>

/**
 * Opaque structure containing internal data.
 */
struct BestFSState;
typedef struct BestFSState BestFSState;

/**
 * Allocates a new BestFS object.
 * 
 * @returns the allocated object.
 */
EXPORT_API MARK_OBJ_ALLOC BestFSState* bestfs_create(const ISearchProblem* problem, const void* initial_state) MARK_NONNULL_ARGS(1);

/**
 * Perform a single BestFS iteration.
 * 
 * @param bfs \ref bestfs_create.
 * @param goal_state where to write the goal state found.
 * @returns 0 if no goal has been reached.
 *   If a goal has been reached, then it writes that state into goal_state and returns 1.
 *   If all the nodes have been searched, then it returns -1.
 *   Any other value indicates a possibly fatal error.
 */
EXPORT_API int bestfs_step(BestFSState* bfs, void* goal_state) MARK_NONNULL_ARGS(1);

/**
 * Deallocates a previously allocated BestFS object.
 * 
 * @param bfs \ref bestfs_create.
 */
EXPORT_API void bestfs_destroy(BestFSState* bfs) MARK_NONNULL_ARGS(1);

#endif /*SSCE_SEARCH_BESTFIRST_H*/