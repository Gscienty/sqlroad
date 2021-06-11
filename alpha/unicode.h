#ifndef __SQLROAD_ALPHA_UNICODE_H__
#define __SQLROAD_ALPHA_UNICODE_H__

#include "common/platform.h"
#include <stdint.h>
#include <stddef.h>

typedef uint32_t sr_unicode_t;
typedef enum sr_encode_e sr_encode_t;
enum sr_encode_e {
    sr_encode_utf8,
};

#define sr_unicode_eof ((sr_unicode_t) (-1))

sr_unicode_t sr_unicode_code_at(const uint8_t *const raw, size_t off, const sr_encode_t et);

size_t sr_unicode_width(const sr_unicode_t code, const sr_encode_t et);

_Bool sr_unicode_is_space(const sr_unicode_t code);

#endif
