/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#include "common/string.h"
#include "common/platform.h"
#include "common/container.h"
#include <string.h>

sr_string_t *sr_string(const char *const str) {
    size_t str_l = strlen(str);
    sr_string_t *ret = sr_malloc(str_l + sizeof(sr_string_t));
    if (!ret) {
        return NULL;
    }

    sr_capacity(ret) = str_l;
    sr_len(ret) = str_l;
    memcpy(ret->data, str, ret->len);

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

int sr_string_trim_start(sr_string_t *const str, const uint8_t alpha) {
    size_t trim_l = 0;
    uint8_t c;
    sr_foreach(c, str) {
        if (c != alpha) {
            break;
        }
        trim_l++;
    }

    if (trim_l == 0 && sr_string_charat(str, trim_l) != alpha) {
        return 0;
    }
    memmove(sr_string_raw(str), sr_string_raw(str) + trim_l, sr_len(str) - trim_l);
    sr_len(str) -= trim_l;

    return 0;
}

int sr_string_trim_end(sr_string_t *const str, const uint8_t alpha) {
    size_t len = sr_len(str);
    uint8_t c;
    sr_reverse_foreach(c, str) {
        if (c != alpha) {
            break;
        }
        len--;
    }

    sr_len(str) = len;

    return 0;
}
