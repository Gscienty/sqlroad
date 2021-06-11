#ifndef __SQLROAD_COLLECTION_INTERFACE_H__
#define __SQLROAD_COLLECTION_INTERFACE_H__

#include "collection/string.h"
#include "collection/link.h"

#define sr_len(ptr) \
    _Generic(ptr, \
             sr_string_t *: sr_string_len(ptr))

typedef union sr_each_s sr_each_t;

union sr_each_s {
    sr_string_each_t string;
    sr_link_each_t link;
};

#define sr_each_init(each, coll, reverse) \
    _Generic(coll, \
             sr_string_t *: sr_string_each_init((sr_string_each_t *) (each), (sr_string_t *) (coll), reverse), \
             sr_link_t *: sr_link_each_init((sr_link_each_t *) (each), (sr_link_t *) (coll), reverse))

#define sr_each_next(each, coll) \
    _Generic(coll, \
             sr_string_t *: sr_string_each_next((sr_string_each_t *) (each)), \
             sr_link_t *: sr_link_each_next((sr_link_each_t *) (each)))

#define sr_each_end(each, coll) \
    _Generic(coll, \
             sr_string_t *: sr_string_each_end((sr_string_each_t *) (each)), \
             sr_link_t *: sr_link_each_end((sr_link_each_t *) (each)))

#define _CAT(a, b) _CAT_I(a, b)
#define _CAT_I(a, b) _CAT_II(~, a ## b)
#define _CAT_II(p, res) res
#define UNIQUE_NAME(base) _CAT(base, __LINE__)

#define sr_each_base(tgt, coll, reverse)            \
    sr_each_t UNIQUE_NAME(each);      \
    sr_each_init(&UNIQUE_NAME(each), coll, reverse); \
    for ((tgt) = (typeof(tgt)) sr_each_next(&UNIQUE_NAME(each), coll); \
         !sr_each_end(&UNIQUE_NAME(each), coll); \
         (tgt) = (typeof(tgt)) sr_each_next(&UNIQUE_NAME(each), coll))

#define sr_each(tgt, coll) sr_each_base(tgt, coll, 0)

#define sr_reach(tgt, coll) sr_each_base(tgt, coll, 1)

#endif
