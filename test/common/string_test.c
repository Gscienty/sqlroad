#include "alpha/unicode.h"
#include "collection/interface.h"
#include "collection/string.h"
#include <stdio.h>

int main() {
    sr_string_t *s = sr_string("              你好世界........................", sr_encode_utf8);

    sr_string_trim_start(s, ' ');
    sr_string_trim_end(s, '.');

    sr_unicode_t code;
    sr_reach(code, s) {
        printf("%x ", code);
    }
    printf("\n");
}
