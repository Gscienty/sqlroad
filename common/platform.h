/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#ifndef __SQLROAD_COMMON_PLATFORM_H__
#define __SQLROAD_COMMON_PLATFORM_H__

#include <stddef.h>
#include <sys/cdefs.h>

void *sr_malloc(const size_t size);

void *sr_realloc(void *const ptr, const size_t size);

int sr_free(void *const ptr);

#ifdef __APPLE__
#define sr_inline_static __header_always_inline
#endif

#ifdef __linux__
#define sr_inline_static static inline
#endif

#endif
