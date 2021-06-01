#ifndef __SQLROAD_COMMON_STRING_H__
#define __SQLROAD_COMMON_STRING_H__

#include <stdint.h>
#include <stddef.h>

typedef struct sr_string_s sr_string_t;

typedef struct sr_string_iterator_s sr_string_iterator_t;

struct sr_string_s {
    size_t capacity;
    size_t len;

    uint8_t data[0];
};

struct sr_string_iterator_s {
    const sr_string_t *str;

    _Bool reverse;
    uint8_t *ptr;
};

sr_string_t *sr_string(const char *const str);

sr_string_t *sr_string_resize(sr_string_t *const str, const size_t capacity);

int sr_string_trim_start(sr_string_t *const str, const uint8_t alpha);

int sr_string_trim_end(sr_string_t *const str, const uint8_t alpha);

#define sr_string_capacity(str) ((str)->capacity)

#define sr_string_len(str) ((str)->len)

#define sr_string_empty(str) (sr_string_len(str) == 0)

#define sr_string_raw(str) ((str)->data)

#define sr_string_charat(str, i) ((i) < sr_string_len(str) ? sr_string_raw(str)[i] : -1)

#define sr_string_trim(str, alpha) {  \
    sr_string_trim_start(str, alpha); \
    sr_string_trim_end(str, alpha);   \
}

#define sr_string_iterator_end(iterator)                         \
    ((iterator).reverse                                         \
     ? ((iterator).ptr + 1) == sr_string_raw((iterator).str) \
     : (iterator).ptr == (sr_string_raw((iterator).str) + sr_string_len((iterator).str)))

#define sr_string_iterator_next(iterator) \
    ((iterator).reverse ? *((iterator).ptr--) : *((iterator).ptr++))

#define sr_string_iterator_init(_str, _reverse) ((sr_string_iterator_t) {                 \
    .str = _str,                                                                          \
    .reverse = _reverse,                                                                  \
    .ptr = _reverse ? sr_string_raw(_str) + sr_string_len(_str) - 1 : sr_string_raw(_str) \
})

#endif
