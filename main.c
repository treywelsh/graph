#include <stdio.h>

#include <err.h>

#include "graph.h"
#include "graph_traversal.h"

/* test cases */
int test_case_loop(void);
int test_case_topo_sort(void);
int test_case_dfs(void);

int
test_case_loop(void) {
    uint32_t i;
    struct graph g;

    graph_init(&g, 20, 20);

    for (i = 0 ; i < 10 ; i++) {
        graph_add_node(&g);
    }

    graph_add_edge(&g, 3, 1);
    graph_add_edge(&g, 1, 5);
    graph_add_edge(&g, 1, 2);
    graph_add_edge(&g, 5, 4);
    graph_add_edge(&g, 5, 2);

    /* TEST LOOP */
    printf("add loop and make dfs :\n");
    graph_add_edge(&g, 4, 3);
    for (i = 0; i < graph_nodes_count(&g); i++) {
        if (graph_is_cyclic(&g, i)) {
            printf("--> loop detected %d\n", i);
            break;
        }
    }

    printf("remove loop and make dfs :\n");
    graph_remove_edge(&g, 4, 3);
    for (i = 0; i < graph_nodes_count(&g); i++) {
        if (graph_is_cyclic(&g, i)) {
            printf("--> error : loop detected %d\n", i);
            break;
        }
    }

    graph_clean(&g);

    return 0;
}

int
test_case_topo_sort(void) {
    uint32_t i;
    uint32_t nds[4];
    uint32_t nds_cnt;
    uint32_t id;
    struct graph g;
    struct graph_tsort_iter * tsort_it = NULL;

    graph_init(&g, 20, 20);

    printf("adding nodes...\n");
    for (i = 0 ; i < 10 ; i++) {
        id = graph_add_node(&g);
        printf("new node id : %u\n", id);
    }

    printf("adding edges...\n");
    id = graph_add_edge(&g, 3, 1);
    printf("new edge id : %u\n", id);
    graph_add_edge(&g, 1, 5);
    graph_add_edge(&g, 1, 2);
    graph_add_edge(&g, 5, 4);
    graph_add_edge(&g, 5, 2);
    graph_add_edge(&g, 4, 8);

    /* TEST DFS  */

    printf("topo sort greedy (3 elements max at a time) :\n");

    if (graph_tsort_create(&g, &tsort_it)) {
        printf("graph_tsort_create failed, maybe a cycle in graph ?\n");
        return 1;
    }
    while ((nds_cnt = graph_tsort_next(tsort_it, nds, 4)) > 0) {
        for (i = 0; i < nds_cnt; i++) {
            printf("%u ", nds[i]);
        }
        printf("\n");
    }
    graph_tsort_destroy(tsort_it);

    printf("remove edge 4->8 and make same topo sort :\n");
    graph_remove_edge(&g, 4, 8);

    if (graph_tsort_create(&g, &tsort_it)) {
        printf("graph_tsort_create failed, maybe a cycle in graph ?\n");
        return 1;
    }
    while ((nds_cnt = graph_tsort_next(tsort_it, nds, 4)) > 0) {
        for (i = 0; i < nds_cnt; i++) {
            printf("%u ", nds[i]);
        }
        printf("\n");
    }

    graph_tsort_destroy(tsort_it);
    graph_clean(&g);

    return 0;
}

int
test_case_dfs(void) {
    uint32_t i;
    uint32_t nds_cnt;
    struct graph g;
    struct graph_dfs_iter * dfs_it = NULL;
    uint32_t nds[4];

    graph_init(&g, 20, 20);

    for (i = 0 ; i < 10 ; i++) {
        graph_add_node(&g);
    }

    graph_add_edge(&g, 3, 1);
    graph_add_edge(&g, 1, 5);
    graph_add_edge(&g, 1, 2);
    graph_add_edge(&g, 5, 4);
    graph_add_edge(&g, 5, 2);
    graph_add_edge(&g, 2, 1);

    printf("List elements with dfs :\n");

    if (graph_dfs_create(&g, &dfs_it, 3)) {
        printf("graph_dfs_create failed, maybe a cycle in graph ?\n");
        return 1;
    }
    while ((nds_cnt = graph_dfs_next(dfs_it, nds, 4)) > 0) {
        for (i = 0; i < nds_cnt; i++) {
            printf("%u ", nds[i]);
        }
        printf("\n");
    }

    graph_dfs_destroy(dfs_it);
    graph_clean(&g);

    return 0;
}


INIT_ERR();

int
main (int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SET_ERR(stdout);

    printf("\n=== Test case 1\n");
    test_case_loop();

    printf("\n=== Test case 2\n");
    test_case_topo_sort();

    printf("\n=== Test case 3\n");
    test_case_dfs();

    return 0;
}
