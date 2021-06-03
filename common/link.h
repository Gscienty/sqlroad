/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#ifndef __SQLROAD_COMMON_LINK_H__
#define __SQLROAD_COMMON_LINK_H__

#include "common/platform.h"
#include <stddef.h>

typedef struct sr_link_s sr_link_t;
typedef struct sr_lnkele_s sr_lnkele_t;
typedef struct sr_link_iterator_s sr_link_iterator_t;

#define SQLROAD_LINK_ELEMENT \
    sr_link_t *link; \
    sr_lnkele_t *prev; \
    sr_lnkele_t *next; \

struct sr_lnkele_s { SQLROAD_LINK_ELEMENT };

struct sr_link_s {
    SQLROAD_LINK_ELEMENT

    size_t cnt;
};

struct sr_link_iterator_s {
    _Bool reverse;

    sr_lnkele_t *ele;
};

#define sr_link_insert_after(ele, nele) {        \
    sr_link_len((ele)->link)++;                  \
    (nele)->link = (ele)->link;                  \
    (nele)->next = (ele)->next;                  \
    (nele)->next->prev = (sr_lnkele_t *) (nele); \
    (ele)->next = (sr_lnkele_t *) (nele);        \
    (nele)->prev = (sr_lnkele_t *) (ele);        \
}

#define sr_link_insert_before(ele, nele) {       \
    sr_link_len((ele)->link)++;                  \
    (nele)->link = (ele)->link;                  \
    (nele)->prev = (ele)->prev;                  \
    (nele)->prev->next = (sr_lnkele_t *) (nele); \
    (ele)->prev = (sr_lnkele_t *) (nele);        \
    (nele)->next = (sr_lnkele_t *) (ele);        \
}

#define sr_link_next(ele) (ele)->next

#define sr_link_prev(ele) (ele)->prev

#define sr_lnkele_init(_link, ele) {     \
    (ele)->link = (_link);               \
    (ele)->prev = (sr_lnkele_t *) (ele); \
    (ele)->next = (sr_lnkele_t *) (ele); \
}

#define sr_link_init(lnk) {   \
    sr_lnkele_init(lnk, lnk); \
    sr_link_len(lnk) = 0;     \
}

sr_inline_static size_t *sr_link_capacity_impl(sr_link_t *const lnk) { return &lnk->cnt; }

sr_inline_static size_t *sr_link_len_impl(sr_link_t *const lnk) { return &lnk->cnt; }

sr_inline_static void sr_link_iterator_init_impl(sr_link_iterator_t *const iterator, sr_link_t *const link, _Bool reverse) {
    *iterator = (sr_link_iterator_t) { .reverse = reverse, .ele = (sr_lnkele_t *) link }; 
}

sr_inline_static void *sr_link_iterator_next_impl(sr_link_iterator_t *const iterator) {
    iterator->reverse
        ? (iterator->ele = sr_link_prev(iterator->ele))
        : (iterator->ele = sr_link_next(iterator->ele));

    return &iterator->ele;
}

sr_inline_static _Bool sr_link_iterator_end_impl(sr_link_iterator_t *const iterator) { 
    return iterator->ele == (sr_lnkele_t *) iterator->ele->link;
}

#define sr_link_capacity(ptr) (*sr_link_capacity_impl((sr_link_t *) (ptr)))

#define sr_link_len(ptr) (*sr_link_len_impl((sr_link_t *) (ptr)))

#define sr_link_iterator_init(iterator, link, reverse) \
    sr_link_iterator_init_impl((sr_link_iterator_t *) (iterator), (sr_link_t *) (link), reverse)

#define sr_link_iterator_next(iterator) \
    sr_link_iterator_next_impl((sr_link_iterator_t *) (iterator))

#define sr_link_iterator_end(iterator) \
    sr_link_iterator_end_impl((sr_link_iterator_t *) (iterator))

#endif
