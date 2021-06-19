#include "alpha/unicode.h"
#include "sql/lexer.h"
#include "collection/interface.h"
#include <stdio.h>

int main() {
    sr_link_t tokens;
    sr_link_init(&tokens);

    sr_string_t *sql = sr_string("select a, b, c from d", sr_encode_utf8);
    sr_lexer(&tokens, sql);

    sr_token_t *token;
    sr_each(token, &tokens) {
        printf("%d\n", token->type);
    }

    return 0;
}
