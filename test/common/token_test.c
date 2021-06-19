#include "sql/lexer.h"
#include "collection/interface.h"
#include <stdio.h>

int main() {
    sr_string_t *sql = sr_string("select a, b, c, 你好, 世界 from t1", sr_encode_utf8);
    sr_link_t tokens;
    sr_link_init(&tokens);

    sr_lexer(&tokens, sql);

    sr_token_t *token = NULL;
    sr_each(token, &tokens) {
        if (token->type == sr_token_identity) {
            printf("%.*s\n", (int) sr_string_len(token->token), sr_string_raw(token->token));
        }
    }

    return 0;
}
