/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#include "common/string.h"
#include "common/encode.h"
#include "common/link.h"
#include "common/platform.h"
#include "common/container.h"
#include <string.h>

sr_inline_static sr_utf8_alpha_t sr_string_utf8_alpha_at(const sr_string_t *const str, const size_t off);

sr_string_t *sr_string(const char *const str, const sr_encode_t encode_type) {
    size_t str_l = strlen(str);
    sr_string_t *ret = sr_malloc(str_l + sizeof(sr_string_t));
    if (!ret) {
        return NULL;
    }

    sr_string_encode_type(ret) = encode_type;
    sr_capacity(ret) = str_l;
    sr_len(ret) = str_l;
    memcpy(ret->data, str, sr_len(ret));

    return ret;
}

sr_string_t *sr_string_slice(const sr_string_t *const str, const size_t begin, const size_t end) {
    size_t str_l = end - begin;
    sr_string_t *ret = sr_malloc(str_l + sizeof(sr_string_t));
    if (!ret) {
        return NULL;
    }

    sr_string_encode_type(ret) = sr_string_encode_type(str);
    sr_capacity(ret) = str_l;
    sr_len(ret) = str_l;
    memcpy(ret->data, sr_string_raw(str) + begin, sr_len(ret));

    return ret;
}

sr_string_t *sr_string_resize(sr_string_t *const str, const size_t capacity) {
    if (sr_capacity(str) == capacity) {
        return str;
    }

    sr_string_t *nstr = sr_realloc(str, capacity + sizeof(sr_string_t));

    if (sr_capacity(str) < capacity) {
        sr_capacity(nstr) = capacity;
    }
    else {
        sr_capacity(nstr) = capacity;
        if (sr_len(nstr) > capacity) {
            sr_len(nstr) = capacity;
        }
    }
    return nstr;
}

int sr_string_trim_start(sr_string_t *const str, const sr_alpha_t alpha) {
    size_t trim_l = 0;
    sr_alpha_t c;
    sr_foreach(c, str) {
        if (sr_alpha_cmp(c, alpha, sr_string_encode_type(str)) != 0) {
            break;
        }
        if (trim_l < sr_len(str)) {
            trim_l = sr_string_alpha_next_off(str, trim_l);
        }
    }

    if (trim_l == 0 && sr_alpha_cmp(sr_string_alpha_at(str, trim_l), alpha, sr_string_encode_type(str)) != 0) {
        return 0;
    }
    memmove(sr_string_raw(str), sr_string_raw(str) + trim_l, sr_len(str) - trim_l);
    sr_len(str) -= trim_l;
    return 0;
}

int sr_string_trim_end(sr_string_t *const str, const sr_alpha_t alpha) {
    size_t len = sr_len(str);
    sr_alpha_t c;
    sr_reverse_foreach(c, str) {
        if (sr_alpha_cmp(c, alpha, sr_string_encode_type(str)) != 0) {
            break;
        }
        if (len > 0) {
            len -= sr_alpha_width(c, sr_string_encode_type(str));
        }
    }

    sr_len(str) = len;

    return 0;
}

int sr_string_split(sr_link_t *const lnk, sr_string_t *const str, const sr_alpha_t spliter) {
    size_t start = 0;
    size_t end = 0;

    sr_alpha_t c;
    sr_foreach(c, str) {
        if (sr_alpha_cmp(c, spliter, sr_string_encode_type(str)) == 0) {
            if (start != end) {
                sr_lnkele_string_t *ele = sr_malloc(sizeof(sr_lnkele_string_t));
                if (!ele) {
                    return -1;
                }
                sr_lnkele_init(lnk, ele);

                ele->ele = sr_string_slice(str, start, end);
                if (!ele->ele) {
                    sr_free(ele);
                    return -1;
                }

                sr_link_insert_before(lnk, ele);
            }
            start = end + sr_alpha_width(c, sr_string_encode_type(str));
        }
        end = sr_string_alpha_next_off(str, end);
    }

    return 0;
}

size_t sr_string_index_of(sr_string_t *const str, const sr_alpha_t alpha) {
    size_t off = 0;
    sr_alpha_t c;
    sr_foreach(c, str) {
        if (sr_alpha_cmp(c, alpha, sr_string_encode_type(str)) == 0) {
            return off;
        }
        if (off < sr_len(str)) {
            off = sr_string_alpha_next_off(str, off);
        }
    }
    return -1;
}

size_t sr_string_last_index_of(sr_string_t *const str, const sr_alpha_t alpha) {
    size_t off = sr_len(str) - 1;
    if (!sr_alpha_where_to_start(sr_string_raw(str)[off], sr_string_encode_type(str))) {
        off = sr_string_alpha_prev_off(str, off);
    }

    sr_alpha_t c;
    sr_reverse_foreach(c, str) {
        if (sr_alpha_cmp(c, alpha, sr_string_encode_type(str)) == 0) {
            return off;
        }
        if (off > 0) {
            off = sr_string_alpha_prev_off(str, off);
        }
    }
    return -1;
}

sr_inline_static sr_utf8_alpha_t sr_string_utf8_alpha_at(const sr_string_t *const str, const size_t off) {
    if (sr_string_len(str) <= off) {
        return -1;
    }
    size_t width = 0;
    uint8_t first_byte = sr_string_raw(str)[off];
    if (0xf0 == (0xf8 & first_byte)) {
        width = 4;
    }
    else if (0xe0 == (0xf0 & first_byte)) {
        width = 3;
    }
    else if (0xc0 == (0xe0 & first_byte)) {
        width = 2;
    }
    else {
        width = 1;
    }
    
    if (sr_string_len(str) <= off + width - 1) {
        return -1;
    }

    switch (width) {
    case 1:
        return sr_string_raw(str)[off];
    case 2:
        return ((uint32_t) sr_string_raw(str)[off])
            | (((uint32_t) sr_string_raw(str)[off + 1]) >> 8);
    case 3:
        return ((uint32_t) sr_string_raw(str)[off])
            | (((uint32_t) sr_string_raw(str)[off + 1]) >> 8)
            | (((uint32_t) sr_string_raw(str)[off + 2]) >> 16);
    case 4:
        return ((uint32_t) sr_string_raw(str)[off])
            | (((uint32_t) sr_string_raw(str)[off + 1]) >> 8)
            | (((uint32_t) sr_string_raw(str)[off + 2]) >> 16)
            | (((uint32_t) sr_string_raw(str)[off + 3]) >> 24);
    }

    return -1;
}

sr_alpha_t sr_string_alpha_at(const sr_string_t *const str, const size_t off) {
    switch (sr_string_encode_type(str)) {
    case sr_encode_ascii:
        return (sr_alpha_t) { .ascii = sr_string_raw(str)[off] };
    case sr_encode_utf8:
        return (sr_alpha_t) { .utf8 = sr_string_utf8_alpha_at(str, off) };
    }
}

