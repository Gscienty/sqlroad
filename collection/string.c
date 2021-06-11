#include "alpha/unicode.h"
#include "collection/interface.h"
#include "collection/string.h"
#include <string.h>

sr_string_t *sr_string(const char *const raw, const sr_encode_t encode_type) {
    size_t str_l = strlen(raw);
    sr_string_t *ret = sr_malloc(str_l + sizeof(sr_string_t));
    if (!ret) {
        return NULL;
    }

    ret->encode_type = encode_type;
    sr_string_capacity(ret) = str_l;
    sr_len(ret) = str_l;
    memcpy(sr_string_raw(ret), raw, sr_len(ret));

    return ret;
}

sr_string_t *sr_string_slice(const sr_string_t *const str, const size_t begin, const size_t end) {
    size_t str_l = end - begin;
    sr_string_t *ret = sr_malloc(str_l + sizeof(sr_string_t));
    if (!ret) {
        return NULL;
    }

    ret->encode_type = str->encode_type;
    sr_string_capacity(ret) = str_l;
    sr_len(ret) = str_l;
    memcpy(sr_string_raw(ret), sr_string_raw(str) + begin, sr_len(ret));

    return ret;
}

sr_unicode_t sr_string_code_at(sr_string_t *const str, const size_t off) {
    return sr_unicode_code_at(sr_string_raw(str), off, str->encode_type);
}

int sr_string_trim_start(sr_string_t *const str, const sr_unicode_t code) {
    sr_unicode_t scode;
    size_t trim_l = 0;
    const size_t width = sr_unicode_width(code, str->encode_type);

    sr_each(scode, str) {
        if (code != scode) {
            break;
        }
        if (trim_l < sr_len(str)) {
            trim_l += width;
        }
    }
    memmove(sr_string_raw(str), sr_string_raw(str) + trim_l, sr_len(str) - trim_l);
    sr_len(str) -= trim_l;

    return 0;
}

int sr_string_trim_end(sr_string_t *const str, const sr_unicode_t code) {
    size_t len = sr_len(str);
    const size_t width = sr_unicode_width(code, str->encode_type);
    sr_unicode_t scode;
    sr_reach(scode, str) {
        if (scode != code) {
            break;
        }

        if (len > 0) {
            len -= width;
        }
    }

    sr_len(str) = len;

    return 0;
}
