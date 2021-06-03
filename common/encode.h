#ifndef __SQLROAD_COMMON_ENCODE_UTF8_H__
#define __SQLROAD_COMMON_ENCODE_UTF8_H__

#include "common/platform.h"
#include <stdint.h>
#include <stddef.h>

typedef uint32_t sr_utf8_alpha_t;
typedef enum sr_encode_e sr_encode_t;
typedef union sr_alpha_u sr_alpha_t;

enum sr_encode_e {
    sr_encode_ascii,
    sr_encode_utf8,
};

union sr_alpha_u {
    uint8_t ascii;
    uint32_t utf8;
};

sr_inline_static size_t sr_alpha_utf8_width(sr_utf8_alpha_t alpha) {
    if (0 == (0xffffff80 & alpha)) {
        return 1;
    }
    else if (0 == (0xfffff800 & alpha)) {
        return 2;
    }
    else if (0 == (0xffff0000 & alpha)) {
        return 3;
    }
    else {
        return 4;
    }
}

sr_inline_static sr_alpha_t sr_alpha_ascii(uint8_t alpha) {
    return (sr_alpha_t) { .ascii = alpha };
}

sr_inline_static sr_alpha_t sr_alpha_utf8(sr_utf8_alpha_t alpha) {
    return (sr_alpha_t) { .utf8 = alpha };
}

sr_inline_static size_t sr_alpha_width(const sr_alpha_t alpha, sr_encode_t encode_type) {
    switch (encode_type) {
    case sr_encode_ascii:
        return 1;
    case sr_encode_utf8:
        return sr_alpha_utf8_width(alpha.utf8);
    default:
        return 1;
    }
}

sr_inline_static int sr_alpha_cmp(const sr_alpha_t a, const sr_alpha_t b, const sr_encode_t encode_type) {
    switch (encode_type) {
    case sr_encode_ascii:
        return a.ascii - b.ascii;
    case sr_encode_utf8:
        return a.utf8 - b.utf8;
    }
}

sr_inline_static _Bool sr_alpha_where_to_start(const uint8_t byte, const sr_encode_t encode_type) {
    switch (encode_type) {
    case sr_encode_ascii:
        return 1;
    case sr_encode_utf8:
        return 0x80 != (0xc0 & byte);
    }
}

#endif
