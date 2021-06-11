#include  "alpha/unicode.h"

static size_t sr_utf8_width(const uint8_t first_byte);
static size_t sr_unicode_utf8_at(const uint8_t *const raw, size_t off);

static size_t sr_unicode_utf8_width(const sr_unicode_t code);

sr_unicode_t sr_unicode_code_at(const uint8_t *const raw, size_t off, const sr_encode_t et) {
    switch (et) {
    case sr_encode_utf8:
        return sr_unicode_utf8_at(raw, off);
    }
}

size_t sr_unicode_width(const sr_unicode_t code, const sr_encode_t et) {
    switch (et) {
    case sr_encode_utf8:
        return sr_unicode_utf8_width(code);
    }
}

_Bool sr_unicode_is_space(const sr_unicode_t code) {
    static const sr_unicode_t space[] = { '\t', '\n', '\v', '\f', '\r', ' ', 0x85, 0xa0, 0x200e, 0x200f, 0x2028, 0x2029, 0x00 };

    int i;
    for (i = 0; space[i]; i++) {
        if (code == space[i]) {
            return 1;
        }
    }
    return 0;
}

static size_t sr_unicode_utf8_at(const uint8_t *const raw, size_t off) {
    size_t code_width = sr_utf8_width(raw[off]);
    sr_unicode_t code = raw[off] & (0xff >> code_width);
    off++;

    size_t i;
    for (i = 1; i < code_width; i++) {
        code = (code << 6) | (raw[off] & 0x3f);
        off++;
    }

    return code;
}

static size_t sr_utf8_width(const uint8_t first_byte) {
    static uint8_t _map[256] = { 0 };
    static _Bool _inited = 0;

    if (!_inited) {
        int i;
        for (i = 0; i < 256; i++) {
            if (i < 0x80) {
                _map[i] = 1;
            }
            else if (0xc0 <= i && i < 0xe0) {
                _map[i] = 2;
            }
            else if (0xe0 <= i && i < 0xf0) {
                _map[i] = 3;
            }
            else if (0xf0 <= i && i < 0xf8) {
                _map[i] = 4;
            }
            else if (0xf8 <= i && i < 0xfc) {
                _map[i] = 5;
            }
            else if (0xfc <= i && i < 0xfe) {
                _map[i] = 6;
            }
            else {
                _map[i] = 0;
            }
        }

        _inited = 1;
    }

    return _map[first_byte];
}

static size_t sr_unicode_utf8_width(const sr_unicode_t code) {
    if (code < (1 << 7)) {
        return 1;
    }
    else if (code < (1 << 11)) {
        return 2;
    }
    else if (code < (1 << 16)) {
        return 3;
    }
    else if (code < (1 << 21)) {
        return 4;
    }
    else if (code < (1 << 26)) {
        return 5;
    }
    else {
        return 6;
    }
}
