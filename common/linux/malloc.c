/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#include "common/platform.h"
#include <malloc.h>

void *sr_malloc(const size_t size) {
    return malloc(size);
}

void *sr_realloc(void *const ptr, const size_t size) {
    return realloc(ptr, size);
}

int sr_free(void *const ptr) {
    free(ptr);
    return 0;
}
