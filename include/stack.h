#ifndef STACK_H_
#define STACK_H_

#include <stdlib.h>

struct stack {
    unsigned int *v;
    unsigned v_len;
    unsigned last;
};
typedef struct stack stack_t;

#define stack_is_empty(s) \
    ((s)->last == 0)

#define stack_is_full(s) \
    ((s)->last == (s)->v_len)

#define stack_size(s) \
    ((s)->last)

#define stack_push(s, e) \
    (s)->v[((s)->last)++] = (e)

#define stack_pop(s) \
    ((s)->v[--((s)->last)])

#define stack_get_last(s) \
    ((s)->v[((s)->last) - 1]);

/* Safe operations
 *
 * No memory errors will occur, but
 * with these operations, you vill not see if you pop an empty stack
 * or if you push a full stack without an additionnal test on
 * stack_is_{empty,full} or without adding a printf to theses macros.
 *
 */
#define stack_safe_push(s, e) \
    if (!stack_is_full(s)) { \
        stack_push((s), (e)); \
    }

#define stack_safe_pop(s, ret) \
    if (!stack_is_empty(s)) { \
        ret = stack_pop((s)); \
    }

#define stack_safe_get_last(s, ret) \
    if (!stack_is_empty(s)) { \
        ret = stack_get_last((s); \
    }

static inline int
stack_init(struct stack * st, unsigned int sz) {
    if (st == NULL) {
        return 1;
    } else if (sz  < 2) {
        return 1;
    }

    st->v = malloc(sizeof(*(st->v)) * sz);
    if (st->v == NULL) {
       return 1;
    }
    st->v_len = sz;
    st->last = 0;

    return 0;
}

static inline void
stack_clean(struct stack * st) {
    if (st == NULL) {
        return;
    }
    free(st->v);
}

#endif /* STACK_H_ */
