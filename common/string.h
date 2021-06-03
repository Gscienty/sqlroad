/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#ifndef __SQLROAD_COMMON_STRING_H__
#define __SQLROAD_COMMON_STRING_H__

#include "common/platform.h"
#include "common/link.h"
#include "common/encode.h"
#include <stdint.h>
#include <stddef.h>

typedef struct sr_string_s sr_string_t;
typedef struct sr_string_iterator_s sr_string_iterator_t;
typedef struct sr_lnkele_string_s sr_lnkele_string_t;

struct sr_string_s {
    sr_encode_t encode_type;
    size_t capacity;
    size_t len;

    uint8_t data[0];
};

struct sr_string_iterator_s {
    const sr_string_t *str;

    _Bool reverse;
    _Bool first;
    
    size_t off;
    sr_alpha_t alpha;
};

struct sr_lnkele_string_s {
    SQLROAD_LINK_ELEMENT

    sr_string_t *ele;
};

sr_string_t *sr_string(const char *const str, const sr_encode_t encode_type);

sr_string_t *sr_string_slice(const sr_string_t *const str, const size_t begin, const size_t end);

sr_string_t *sr_string_resize(sr_string_t *const str, const size_t capacity);

int sr_string_trim_start(sr_string_t *const str, const sr_alpha_t alpha);

int sr_string_trim_end(sr_string_t *const str, const sr_alpha_t alpha);

int sr_string_split(sr_link_t *const lnk, sr_string_t *const str, const sr_alpha_t spliter);

size_t sr_string_index_of(sr_string_t *const str, const sr_alpha_t alpha);

size_t sr_string_last_index_of(sr_string_t *const str, const sr_alpha_t alpha);

sr_inline_static size_t *sr_string_capacity_impl(sr_string_t *const ptr) { return &ptr->capacity; }

sr_inline_static size_t *sr_string_len_impl(sr_string_t *const ptr) { return &ptr->len; }

#define sr_string_capacity(ptr) (*sr_string_capacity_impl((sr_string_t *) (ptr)))

#define sr_string_len(ptr) (*sr_string_len_impl((sr_string_t *) (ptr)))

#define sr_string_raw(str) ((str)->data)

#define sr_string_encode_type(str) ((str)->encode_type)

#define sr_string_trim(str, alpha) {  \
    sr_string_trim_start(str, alpha); \
    sr_string_trim_end(str, alpha);   \
}

sr_alpha_t sr_string_alpha_at(const sr_string_t *const str, const size_t off);

sr_inline_static size_t sr_string_alpha_next_off(const sr_string_t *const str, size_t off) {
    return off + sr_alpha_width(sr_string_alpha_at(str, off), sr_string_encode_type(str));
}

sr_inline_static size_t sr_string_alpha_prev_off(const sr_string_t *const str, size_t off) {
    return off - sr_alpha_width(sr_string_alpha_at(str, off), sr_string_encode_type(str));
}

sr_inline_static void sr_string_iterator_init_impl(sr_string_iterator_t *const iterator, sr_string_t *const str, _Bool reverse) {
    size_t off = 0;
    if (reverse && sr_string_len(str) > 0) {
        off = sr_string_len(str) - 1;
        if (!sr_alpha_where_to_start(sr_string_raw(str)[off], sr_string_encode_type(str))) {
            off = sr_string_alpha_prev_off(str, off);
        }
    }
    *iterator = (sr_string_iterator_t) {
        .reverse = reverse,
        .str = str,
        .off = off,
        .first = 1,
        .alpha = sr_string_alpha_at(str, off),
    };
}

sr_inline_static void *sr_string_iterator_next_impl(sr_string_iterator_t *const iterator) {
    if (!iterator->first) {
        iterator->off = iterator->reverse
            ? sr_string_alpha_prev_off(iterator->str, iterator->off)
            : sr_string_alpha_next_off(iterator->str, iterator->off);
    }
    iterator->first = 0;
    iterator->alpha = sr_string_alpha_at(iterator->str, iterator->off);
    return &iterator->alpha;
}

sr_inline_static _Bool sr_string_iterator_end_impl(sr_string_iterator_t *const iterator) {
    return iterator->reverse ? iterator->off <= 0 : sr_string_len(iterator->str) <= iterator->off;
}

#define sr_string_iterator_end(iterator) \
    sr_string_iterator_end_impl((sr_string_iterator_t *) (iterator))

#define sr_string_iterator_next(iterator) \
    sr_string_iterator_next_impl((sr_string_iterator_t *) (iterator))

#define sr_string_iterator_init(iterator, str, reverse) \
    sr_string_iterator_init_impl((sr_string_iterator_t *) (iterator), (sr_string_t *) str, reverse)

#endif
