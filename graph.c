#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <err.h>
#include <stack.h>

#include "errc.h"
#include "graph.h"

/*
 * See another graph implem : https://rosettacode.org/wiki/Dijkstra's_algorithm#C
 */

int
graph_init(struct graph* g, uint32_t nodes_max, uint32_t edges_max) {
    uint32_t i;

    assert(g != NULL);
    assert(nodes_max > 0);
    assert(edges_max > 0);

    g->nds_max = nodes_max;
    g->edgs_max = edges_max;

    /* count special nodes before the first node */
    g->nds_max += GRAPH_ND_FIRST;
    g->edgs_max += GRAPH_EDG_FIRST;

    /* first edge leaving node */
    g->edg_first = malloc(g->nds_max * sizeof(*(g->edg_first)));
    if (g->edg_first == NULL) {
        return GRAPH_MALLOC_ERROR;
    }
    memset(g->edg_first, GRAPH_EDG_NULL, g->nds_max * sizeof(*(g->edg_first)));

    /* next edge leaving node */
    g->edgs_nxt = malloc(g->edgs_max * sizeof(*(g->edgs_dst)));
    if (g->edgs_nxt == NULL) {
        return GRAPH_MALLOC_ERROR;
    }

    /* contain the node destination of an arc */
    g->edgs_dst = malloc(g->edgs_max * sizeof(*(g->edgs_dst)));
    if (g->edgs_nxt == NULL) {
        return GRAPH_MALLOC_ERROR;
    }

    /* edges data */
    g->edgs = malloc(g->edgs_max * sizeof(*(g->edgs)));
    if (g->edgs == NULL) {
        return GRAPH_MALLOC_ERROR;
    }
    edges_reset_seen(g);

    /* nodes data */
    g->nds = malloc(g->nds_max * sizeof(*(g->nds)));
    if (g->nds == NULL) {
        return GRAPH_MALLOC_ERROR;
    }
    nodes_reset_seen(g);

    /* nodes free list */
    g->nds_free = malloc(g->nds_max * sizeof(*g->nds_free));
    if (g->nds_free == NULL) {
        return GRAPH_MALLOC_ERROR;
    }

    g->edgs_count = 0;
    g->edgs_free = GRAPH_EDG_FIRST;
    for (i = GRAPH_EDG_FIRST ; i < edges_max ; i++) {
        edge_init(g, i);
        g->edgs_nxt[i] = i + 1;
    }

    (g->nds_free)[GRAPH_ND_NULL] = GRAPH_ND_FIRST;
    for (i = GRAPH_ND_FIRST ; i < nodes_max ; i++) {
        (g->nds_free)[i] = i + 1;
        node_init(g, i);
    }
    g->nds_count = 0;

    return SUCCESS;
}

void
graph_clean(struct graph* g) {
    assert(g != NULL);

    free(g->edg_first);
    free(g->edgs_nxt);
    free(g->edgs_dst);
    free(g->nds_free);
    free(g->nds);
    free(g->edgs);
}

/* TODO retrieve id in order to create edges ? */
uint32_t
graph_add_node(struct graph* g) {
    uint32_t nd_id;

    assert(g != NULL);

    if (graph_nodes_full(g)) {
        err_print("graph nodes full\n");
        return GRAPH_ND_NULL;
    }

    /* take node from free list */
    nd_id = g->nds_free[GRAPH_ND_NULL];
    g->nds_free[GRAPH_ND_NULL] = g->nds_free[nd_id];

    node_reset(g, nd_id);
    (g->nds_count)++;

    return nd_id;
}

uint32_t
graph_add_edge(struct graph* g, uint32_t u, uint32_t v) {
    uint32_t edg_id;

    assert(g != NULL);
    assert(!graph_edges_full(g));
    assert(u < graph_nodes_count(g));
    assert(v < graph_nodes_count(g));

    if (graph_edges_full(g)) {
        err_print("graph edges full\n");
        return GRAPH_EDG_NULL;
    }

    /* fetch from free list */
    edg_id = g->edgs_free;
    g->edgs_free = g->edgs_nxt[g->edgs_free];

    /* fill edge linked list */
    (g->edgs_dst)[edg_id] = v;

    (g->edgs_nxt)[edg_id] = (g->edg_first)[u];
    (g->edg_first)[u] = edg_id;

    /* prepare and count it in graph */
    edge_reset(g, edg_id);
    g->edgs_count++;

    return edg_id;
}

int
graph_remove_edge(struct graph* g, uint32_t u, uint32_t v) {
    uint32_t i, i_prev;
    uint32_t edg_id;

    assert(g != NULL);
    assert(!graph_edges_empty(g));
    assert(u < graph_nodes_count(g));
    assert(v < graph_nodes_count(g));

    if (graph_edges_empty(g)) {
        err_print("graph edges empty\n");
        return 1;
    }

    /* retrieve edge_id */
    edg_id = GRAPH_EDG_NULL;
    i = g->edg_first[u];
    i_prev = i;
    while (i != GRAPH_EDG_NULL) {
        if (g->edgs_dst[i] == v) {
            edg_id = i;
            break;
        }
        i_prev = i;
        i = g->edgs_nxt[i];
    }

    assert(edg_id != GRAPH_EDG_NULL);

    g->edgs_count--;

    /* if last added edge */
    if (edg_id == (g->edg_first)[u]) {
        (g->edg_first)[u] = g->edgs_nxt[(g->edg_first)[u]];
        return 0;
    } else {
        g->edgs_nxt[i_prev] = g->edgs_nxt[i];
    }

    /* return to free list */
    g->edgs_nxt[edg_id] = g->edgs_free;
    g->edgs_free = edg_id;

    return 0;
}

int
graph_remove_node(struct graph* g, uint32_t nd) {
    uint32_t i;
    uint32_t nh;

    /* remove each edges leaving node */
    for (i = g->edg_first[nd] ; i != GRAPH_EDG_NULL ; i = g->edgs_nxt[i]) {
        nh = g->edgs_dst[i];

        graph_remove_edge(g, nd, nh);

    }
    node_reset(g, nd);

    /* add element to free list */
    g->nds_free[nd] = g->nds_free[GRAPH_ND_NULL];
    g->nds_free[GRAPH_ND_NULL] = nd;

    (g->nds_count)--;

    return SUCCESS;
}

int
graph_is_cyclic(struct graph* g, uint32_t first_nd) {
    uint32_t nh;
    uint32_t i, i_edg;
    int ret;
    struct stack stk = {0};

    assert(g != NULL);
    assert(first_nd < graph_nodes_count(g));

    stack_init(&stk, graph_nodes_count(g));
    stack_push(&stk, first_nd);

    ret = 0;
    while (!stack_is_empty(&stk)) {
        i = stack_pop(&stk);

        /* detect cycle in marking visited edges :
         * 1. mark each edges leaving node at index i
         * 2. stack each neighbour of node at index i
         */
        i_edg = g->edg_first[i];
        while (i_edg != GRAPH_EDG_NULL) {

            /* visit edge only once */
            if ((g->edgs)[i_edg].seen) {
                ret = 1;
                goto clean;
            }
            (g->edgs)[i_edg].seen = 1;

            /* push node */
            nh = g->edgs_dst[i_edg];
            stack_push(&stk, nh);

            i_edg = g->edgs_nxt[i_edg];
        }
    }

clean:
    edges_reset_seen(g);
    stack_clean(&stk);

    return ret;
}
