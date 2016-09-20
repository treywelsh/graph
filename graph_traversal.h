#ifndef GRAPH_TRAVERSAL_H_
#define GRAPH_TRAVERSAL_H_

#include <stdint.h>

#include "graph.h"

/* Graph traversal */
struct graph_dfs_iter;

int graph_dfs_create(struct graph* g, struct graph_dfs_iter ** it, uint32_t first_nd);
void graph_dfs_destroy(struct graph_dfs_iter * it);
int graph_dfs_next(struct graph_dfs_iter * it, uint32_t * ndret, uint32_t ndsz);


/* Topological sort based on greedy algorithm.
 * It work as an iterator returning several nodes at a time.
 * WARNING: do not modify the graph between
 *      graph_tsort_create and graph_tsort_destroy call.
 */
struct graph_tsort_iter;

int graph_tsort_create(struct graph* g, struct graph_tsort_iter ** it);
void graph_tsort_destroy(struct graph_tsort_iter * it);
uint32_t graph_tsort_next(struct graph_tsort_iter * it, uint32_t * nds,
        uint32_t nds_sz); /* return an array of nodes */

#endif /* GRAPH_TRAVERSAL_H_ */
