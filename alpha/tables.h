/*
 * Copyright (c) 2021 Gscienty <gaoxiaochuan@hotmail.com>
 *
 * Distributed under the MIT software license, see the accompanying
 * file LICENSE or https://www.opensource.org/licenses/mit-license.php .
 *
 */

#ifndef __SQLROAD_ALPHA_TABLES_H__
#define __SQLROAD_ALPHA_TABLES_H__

#include <stdint.h>
#include "alpha/unicode.h"

typedef struct sr_alpha_range_s sr_alpha_range_t;

struct sr_alpha_range_s {
    sr_unicode_t start_range;
    sr_unicode_t end_range;
    uint32_t step;
};

extern const sr_alpha_range_t sr_alpha_letter[];
extern const sr_alpha_range_t sr_alpha_digit[];

_Bool sr_alpha_belong_to(const sr_alpha_range_t ranges[], const sr_unicode_t code);

#endif
