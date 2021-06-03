/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#ifndef __SQLROAD_COMMON_CONTAINER_H__
#define __SQLROAD_COMMON_CONTAINER_H__

#include "common/string.h"
#include "common/link.h"

#define sr_capacity(ptr)                           \
    _Generic(*(ptr),                               \
             sr_string_t: sr_string_capacity(ptr), \
             sr_link_t: sr_link_capacity(ptr),     \
             default: 0)

#define sr_len(ptr)                           \
    _Generic(*(ptr),                          \
             sr_string_t: sr_string_len(ptr), \
             sr_link_t: sr_link_len(ptr),     \
             default: 0)

#define sr_empty(ptr)                           \
    _Generic(*(ptr),                            \
             sr_string_t: sr_string_empty(ptr), \
             sr_link_t: sr_link_empty(ptr),     \
             default: 0)

union sr_iterator_u {
    sr_string_iterator_t string;
    sr_link_iterator_t link;
};

typedef union sr_iterator_u sr_iterator_t;

#define sr_iterator(ptr, iterator)            \
    _Generic(*(ptr),                          \
             sr_string_t: &(iterator).string, \
             sr_link_t: &(iterator).link      \
             )

#define sr_iterator_init(ptr, iterator, reverse)                             \
    _Generic((ptr),                                                          \
             sr_string_t *: sr_string_iterator_init(iterator, ptr, reverse), \
             sr_link_t *: sr_link_iterator_init(iterator, ptr, reverse)      \
             )

#define sr_iterator_next(iterator)                                      \
    _Generic((iterator),                                                \
             sr_string_iterator_t *: sr_string_iterator_next(iterator), \
             sr_link_iterator_t *: sr_link_iterator_next(iterator),     \
             default: NULL)

#define sr_iterator_end(iterator)                                      \
    _Generic((iterator),                                               \
             sr_string_iterator_t *: sr_string_iterator_end(iterator), \
             sr_link_iterator_t *: sr_link_iterator_end(iterator)      \
             )

#define _CAT(a, b) _CAT_I(a, b)
#define _CAT_I(a, b) _CAT_II(~, a ## b)
#define _CAT_II(p, res) res
#define UNIQUE_NAME(base) _CAT(base, __LINE__)

#define sr_foreach_base(eachor, container, reverse)                                                      \
    sr_iterator_t UNIQUE_NAME(iterator);                                                                 \
    sr_iterator_init(container, sr_iterator(container, UNIQUE_NAME(iterator)), reverse);                 \
    for ((eachor) = *(typeof(eachor) *) sr_iterator_next(sr_iterator(container, UNIQUE_NAME(iterator))); \
         !sr_iterator_end(sr_iterator(container, UNIQUE_NAME(iterator)));                                \
         (eachor) = *(typeof(eachor) *) sr_iterator_next(sr_iterator(container, UNIQUE_NAME(iterator))))

#define sr_foreach(eachor, container) sr_foreach_base(eachor, container, 0)

#define sr_reverse_foreach(eachor, container) sr_foreach_base(eachor, container, 1)

#endif
