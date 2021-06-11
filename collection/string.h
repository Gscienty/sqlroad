#ifndef __SQLROAD_COLLECTION_STRING_H__
#define __SQLROAD_COLLECTION_STRING_H__

#include "alpha/unicode.h"
#include "common/platform.h"

typedef struct sr_string_s sr_string_t;
typedef struct sr_string_each_s sr_string_each_t;

struct sr_string_s {
    sr_encode_t encode_type;

    size_t capacity;
    size_t len;

    uint8_t raw[0];
};

struct sr_string_each_s {
    sr_string_t *str;

    _Bool end;
    _Bool reverse;
    size_t off;
};

sr_string_t *sr_string(const char *const raw, const sr_encode_t encode_type);

sr_string_t *sr_string_slice(const sr_string_t *const str, const size_t begin, const size_t end);

sr_unicode_t sr_string_code_at(sr_string_t *const str, const size_t off);

int sr_string_trim_start(sr_string_t *const str, const sr_unicode_t code);

int sr_string_trim_end(sr_string_t *const str, const sr_unicode_t code);

sr_inline_static size_t *sr_string_capacity_impl(sr_string_t *const ptr) { return &ptr->capacity; }

sr_inline_static size_t *sr_string_len_impl(sr_string_t *const ptr) { return &ptr->len; }

#define sr_string_capacity(ptr) (*sr_string_capacity_impl((sr_string_t *) (ptr)))

#define sr_string_len(ptr) (*sr_string_len_impl((sr_string_t *) (ptr)))

#define sr_string_raw(str) ((str)->raw)

sr_inline_static void sr_string_each_init(sr_string_each_t *const each, sr_string_t *const str, _Bool reverse) {
    *each = (sr_string_each_t) { .off = 0, .str = str, .reverse = reverse, .end = sr_string_len(str) == 0 };
}

sr_inline_static _Bool sr_string_each_end(sr_string_each_t *const each) {
    return each->end;
}

sr_inline_static sr_unicode_t sr_string_each_next(sr_string_each_t *const each) {
    if (each->off == sr_string_len(each->str)) {
        each->end = 1;
        return sr_unicode_eof;
    }
    sr_unicode_t code = sr_unicode_eof;
    if (each->reverse) {
        size_t toff = sr_string_len(each->str) - 1 - each->off;
        switch (each->str->encode_type) {
        case sr_encode_utf8:
            while (toff >= 0 && (sr_string_raw(each->str)[toff] & 0xc0) == 0x80) {
                toff--;
                each->off++;
            }
            break;
        }
        each->off++;

        if (toff < 0) {
            return sr_unicode_eof;
        }

        code = sr_string_code_at(each->str, toff);
    }
    else {
        code = sr_string_code_at(each->str, each->off);
        each->off += sr_unicode_width(code, each->str->encode_type);
    }

    return code;
}

#endif
