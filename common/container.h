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

#define sr_capacity(ptr)                             \
    _Generic((ptr),                                  \
             sr_string_t *: sr_string_capacity(ptr), \
             default: 0)

#define sr_len(ptr)                             \
    _Generic((ptr),                             \
             sr_string_t *: sr_string_len(ptr), \
             default: 0)

#define sr_empty(ptr)                             \
    _Generic((ptr),                               \
             sr_string_t *: sr_string_empty(ptr), \
             default: 0)

union sr_iterator_u {
    sr_string_iterator_t string;
};

typedef union sr_iterator_u sr_iterator_t;

#define sr_iterator(ptr, iterator) \
    _Generic((ptr), \
             sr_string_t *: (iterator).string \
             )

#define sr_iterator_init(ptr, reverse) \
    _Generic((ptr), \
             sr_string_t *: sr_string_iterator_init(ptr, reverse) \
             )

#define sr_iterator_next(iterator) \
    _Generic((iterator), \
             sr_string_iterator_t: sr_string_iterator_next(iterator), \
             default: NULL)

#define sr_iterator_end(iterator) \
    _Generic((iterator), \
             sr_string_iterator_t: sr_string_iterator_end(iterator) \
             )

#define _CAT(a, b) _CAT_I(a, b)
#define _CAT_I(a, b) _CAT_II(~, a ## b)
#define _CAT_II(p, res) res
#define UNIQUE_NAME(base) _CAT(base, __LINE__)

#define sr_foreach_base(eachor, container, reverse)                                       \
    sr_iterator_t UNIQUE_NAME(iterator);                                                  \
    sr_iterator(container, UNIQUE_NAME(iterator)) = sr_iterator_init(container, reverse); \
    for ((eachor) = sr_iterator_next(sr_iterator(container, UNIQUE_NAME(iterator)));      \
         !sr_iterator_end(sr_iterator(container, UNIQUE_NAME(iterator)));                 \
         (eachor) = sr_iterator_next(sr_iterator(container, UNIQUE_NAME(iterator))))

#define sr_foreach(eachor, container) sr_foreach_base(eachor, container, 0)

#define sr_reverse_foreach(eachor, container) sr_foreach_base(eachor, container, 1)

#endif
