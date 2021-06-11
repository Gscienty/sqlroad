#ifndef __SQLROAD_COLLECTION_LINK_H__
#define __SQLROAD_COLLECTION_LINK_H__

#include "common/platform.h"
#include <stddef.h>

typedef struct sr_link_s sr_link_t;
typedef struct sr_linker_s sr_linker_t;
typedef struct sr_link_each_s sr_link_each_t;

#define SQLROAD_LINKER \
    sr_link_t *link;   \
    sr_linker_t *prev; \
    sr_linker_t *next; \

struct sr_link_s {
    SQLROAD_LINKER;
    size_t cnt;
};

struct sr_linker_s { SQLROAD_LINKER };

struct sr_link_each_s {
    sr_linker_t *linker;
    _Bool reverse;
};

#define sr_link_init(lnk) {              \
    (lnk)->link = (sr_link_t *) (lnk);   \
    (lnk)->prev = (sr_linker_t *) (lnk); \
    (lnk)->next = (sr_linker_t *) (lnk); \
    (lnk)->cnt = 0;                      \
}

#define sr_linker_init(lnker) { \
    (lnker)->link = NULL; \
    (lnker)->next = (sr_linker_t *) (lnker); \
    (lnker)->prev = (sr_linker_t *) (lnker); \
}

#define sr_link_insert_after(lnker, nlnker) {        \
    (nlnker)->link = (lnker)->link;                  \
    (nlnker)->link->cnt++;                           \
    (nlnker)->next = (lnker)->next;                  \
    (lnker)->next = (sr_linker_t *) (nlnker);        \
    (nlnker)->next->prev = (sr_linker_t *) (nlnker); \
    (nlnker)->prev = (sr_linker_t *) (lnker);         \
}

#define sr_link_insert_before(lnker, nlnker) {       \
    (nlnker)->link = (lnker)->link;                  \
    (nlnker)->link->cnt++;                           \
    (nlnker)->prev = (lnker)->prev;                  \
    (lnker)->prev = (sr_linker_t *) (nlnker);        \
    (nlnker)->prev->next = (sr_linker_t *) (nlnker); \
    (nlnker)->next = (sr_linker_t *) (lnker);        \
}

#define sr_link_next(lnker) ((typeof(lnker)) (lnker)->next)

#define sr_link_prev(lnker) ((typeof(lnker)) (lnker)->prev)

sr_inline_static void sr_link_each_init(sr_link_each_t *const each, sr_link_t *const link, _Bool reverse) {
    *each = (sr_link_each_t) { .linker = (sr_linker_t *) link, .reverse = reverse };
}

sr_inline_static _Bool sr_link_each_end(sr_link_each_t *const each) {
    return each->linker == (sr_linker_t *) each->linker->link;
}

sr_inline_static sr_linker_t *sr_link_each_next(sr_link_each_t *const each) {
    if (each->reverse) {
        each->linker = each->linker->prev;
    }
    else {
        each->linker = each->linker->next;
    }

    return each->linker;
}

#endif
