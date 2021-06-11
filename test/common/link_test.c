#include "alpha/unicode.h"
#include "collection/interface.h"
#include "collection/link.h"
#include "collection/special-list.h"
#include "common/platform.h"
#include <stdio.h>

int main() {
    sr_link_t l;
    sr_link_init(&l);

    sr_linker_string_t *n = sr_malloc(sizeof(sr_linker_string_t));
    sr_linker_init(n);
    n->value = sr_string("Hello", sr_encode_utf8);
    sr_link_insert_prev(&l, n);

    n = sr_malloc(sizeof(sr_linker_string_t));
    sr_linker_init(n);
    n->value = sr_string("World", sr_encode_utf8);
    sr_link_insert_prev(&l, n);


    sr_each(n, &l) {
        printf("%s\n", sr_string_raw(n->value));
    }

    return 0;
}
