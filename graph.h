#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdint.h>

#include <err.h>

/* node data */
struct graph_node {
    uint32_t seen:1;
    uint32_t unused:31;
};

#define node_init(g, i) do{ \
    ((g)->nds)[i].seen = 0; \
}while(0)
#define node_reset node_init

#define nodes_reset_seen(g) \
    for (uint32_t nd_rst_i = GRAPH_ND_FIRST; nd_rst_i < (g)->nds_max ; nd_rst_i++) { \
        ((g)->nds)[nd_rst_i].seen = 0; \
    }

/* edge data */
struct graph_edge {
    uint32_t seen:1;
    uint32_t weight:15;
    uint32_t unused:16;
};

#define edge_init(g, i) do{ \
    ((g)->edgs)[i].seen = 0; \
    ((g)->edgs)[i].weight = 0; \
}while(0)
#define edge_reset edge_init

#define edges_reset_seen(g) \
    for (uint32_t edg_rst_i = GRAPH_EDG_FIRST; edg_rst_i < (g)->edgs_max ; edg_rst_i++) { \
        ((g)->edgs)[edg_rst_i].seen = 0; \
    }

/* This graph implementation store edges adjcencies list
 * in kind of finite linked list data structure based on indexes.
 */
struct graph {

    /* nodes */
    struct graph_node * nds;
    uint32_t nds_count;
    uint32_t nds_max;

    /* edges */
    struct graph_edge * edgs;
    uint32_t edgs_count;
    uint32_t edgs_max;
    uint32_t edgs_free; /* head of free list, which is stored in edgs_nxt.
                         * It require no extra memory space,
                         * just edgs_free. */

    /* edges metadata */
    uint32_t * edg_first; /* first edge leaving node */
    uint32_t * edgs_nxt; /* next edge leaving node. Free elements are linked. */
    uint32_t * edgs_dst; /* contain the node destination of an arc */

    /* nodes metadata */
    uint32_t * nds_free; /* nodes freelist */
};

/* special edges */
enum {
    GRAPH_EDG_NULL = 0,
    GRAPH_EDG_FIRST,
};

/* special nodes */
enum {
    GRAPH_ND_NULL = 0,
    GRAPH_ND_FIRST,
};

#define graph_edges_count(g) ((g)->edgs_count)
#define graph_edges_full(g) ((g)->edgs_count == (g)->edgs_max - GRAPH_EDG_FIRST)
#define graph_edges_empty(g) ((g)->edgs_count == 0)

#define graph_nodes_count(g) ((g)->nds_count)
#define graph_nodes_full(g) ((g)->nds_count == (g)->nds_max - GRAPH_ND_FIRST)
#define graph_nodes_empty(g) ((g)->nds_count == 0)

#define graph_foreach_neighbor(g, nd, nh) \
    for (uint32_t idx = (g)->edg_first[nd] ; \
            idx != GRAPH_EDG_NULL && (nh = (g)->edgs_dst[idx], 1) ; \
            idx = (g)->edgs_nxt[idx])


int graph_init(struct graph* g, uint32_t nodes_max, uint32_t edges_max);
void graph_clean(struct graph* g);

/* Return : id of newly added elements */
uint32_t graph_add_node(struct graph* g);
uint32_t graph_add_edge(struct graph* g, uint32_t src, uint32_t dst);

int graph_remove_edge(struct graph* g, uint32_t src, uint32_t dst);
int graph_remove_node(struct graph* g, uint32_t nd_id);

int graph_is_cyclic(struct graph* g, uint32_t node_first);

#endif /* GRAPH_H_ */
