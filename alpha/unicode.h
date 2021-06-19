#ifndef __SQLROAD_ALPHA_UNICODE_H__
#define __SQLROAD_ALPHA_UNICODE_H__

#include "common/platform.h"
#include <stdint.h>
#include <stddef.h>

typedef uint32_t sr_unicode_t;
typedef enum sr_encode_e sr_encode_t;
typedef enum sr_alpha_property_e sr_alpha_property_t;

enum sr_encode_e {
    sr_encode_utf8,
};

enum sr_alpha_property_e {
    sr_alpha_property_numberal = 0x01,
    sr_alpha_property_control = 0x02,
    sr_alpha_property_punctuation = 0x04,
    sr_alpha_property_symbolic = 0x08,
    sr_alpha_property_spacing = 0x10,
    sr_alpha_property_upper_case = 0x20,
    sr_alpha_property_lower_case = 0x40,
};

#define sr_unicode_eof ((sr_unicode_t) (-1))

sr_unicode_t sr_unicode_code_at(const uint8_t *const raw, size_t off, const sr_encode_t et);

size_t sr_unicode_width(const sr_unicode_t code, const sr_encode_t et);

_Bool sr_unicode_is_space(const sr_unicode_t code);

_Bool sr_unicode_is_alpha_numeric(const sr_unicode_t code);

#endif
