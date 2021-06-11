#ifndef __SQLROAD_SQL_LEXER_H__
#define __SQLROAD_SQL_LEXER_H__

#include "collection/string.h"
#include "collection/link.h"
#include "common/platform.h"

typedef struct sr_token_position_s sr_token_position_t;
typedef struct sr_token_s sr_token_t;
typedef struct sr_lexer_s sr_lexer_t;
typedef struct sr_lexer_fn_s sr_lexer_fn_t;
typedef enum sr_token_type_e sr_token_type_t;

enum sr_token_type_e {
    sr_token_identity,
    sr_token_quote,
    sr_token_operator,
    sr_token_number,
    sr_token_bool,
    sr_token_string,
    sr_token_eof,
};

struct sr_token_position_s {
    size_t line;
    size_t off;
};

struct sr_token_s {
    SQLROAD_LINKER

    sr_string_t *token;
    sr_token_type_t type;
};

struct sr_lexer_fn_s { sr_lexer_fn_t (*fn) (sr_lexer_t *const lexer); };

int sr_lexer(sr_link_t *const tokens, sr_string_t *const source);

#endif
