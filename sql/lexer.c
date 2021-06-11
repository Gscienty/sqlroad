#include "sql/lexer.h"
#include "alpha/unicode.h"
#include "collection/interface.h"
#include "collection/link.h"
#include "collection/string.h"
#include "common/platform.h"

struct sr_lexer_s {
    sr_string_t *source;
    sr_link_t *tokens;
    int error;
    size_t start;
    size_t end;
    size_t width;
    sr_token_position_t local_pos;
    sr_token_position_t prev_pos;
    sr_token_position_t start_pos;
};

sr_inline_static sr_unicode_t sr_lexer_next_alpha(sr_lexer_t *const lexer);
sr_inline_static sr_unicode_t sr_lexer_peek_alpha(sr_lexer_t *const lexer);
sr_inline_static void sr_lexer_ignore(sr_lexer_t *const lexer);
sr_inline_static sr_string_t *sr_lexer_word(sr_lexer_t *const lexer);
static sr_lexer_fn_t sr_lexer_process(sr_lexer_t *const lexer);
static sr_lexer_fn_t sr_lexer_process_eof(sr_lexer_t *const lexer);
static int sr_lexer_scan_string(sr_lexer_t *const lexer, const sr_unicode_t quote);
static sr_unicode_t sr_lexer_scan_escape(sr_lexer_t *const lexer, const sr_unicode_t quote);

int sr_lexer(sr_link_t *const tokens, sr_string_t *const source) {
    sr_lexer_t lexer = {
        .source = source,
        .tokens = tokens,
        .error = 0,
        .start = 0,
        .end = 0,
        .width = 1,
        .start_pos = { .line = 1, .off = 0 },
        .local_pos = { .line = 1, .off = 0 },
        .prev_pos = { .line = 1, .off = 0 }
    };

    sr_lexer_fn_t fn;
    for (fn = (sr_lexer_fn_t) { .fn = sr_lexer_process }; fn.fn; ) {
        fn = fn.fn(&lexer);
    }

    return 0;
}

static sr_lexer_fn_t sr_lexer_process(sr_lexer_t *const lexer) {
    sr_unicode_t alpha = sr_lexer_next_alpha(lexer);

    if (alpha == sr_unicode_eof) {
        return sr_lexer_process_eof(lexer);
    }
    if (sr_unicode_is_space(alpha)) {
        sr_lexer_ignore(lexer);
    }
    if (alpha == '\'' || alpha == '"') {
        if ((lexer->error = sr_lexer_scan_string(lexer, alpha)) != 0) {
            return (sr_lexer_fn_t) { .fn = NULL };
        }
    }

    return (sr_lexer_fn_t) { .fn = NULL };
}

static sr_lexer_fn_t sr_lexer_process_eof(sr_lexer_t *const lexer) {
    sr_token_t *token = sr_malloc(sizeof(sr_token_t));
    sr_linker_init(token);

    token->token = NULL;
    token->type = sr_token_eof;

    sr_link_insert_before(lexer->tokens, token);

    return (sr_lexer_fn_t) { .fn = NULL };
}

static int sr_lexer_scan_string(sr_lexer_t *const lexer, const sr_unicode_t quote) {
    sr_unicode_t alpha = sr_lexer_next_alpha(lexer);
    while (alpha != quote) {
        if (alpha == '\n' || alpha == sr_unicode_eof) {
            return -1;
        }
        if (alpha == '\\') {
            alpha = sr_lexer_scan_escape(lexer, alpha);
            if (alpha == sr_unicode_eof) {
                return -1;
            }
        }
        else {
            alpha = sr_lexer_next_alpha(lexer);
        }
    }

    return 0;
}

static sr_unicode_t sr_lexer_scan_escape(sr_lexer_t *const lexer, const sr_unicode_t quote) {
    sr_unicode_t alpha = sr_lexer_next_alpha(lexer);

    switch (alpha) {
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v':
    case '\\':
        return sr_lexer_next_alpha(lexer);
    }
    if (alpha == quote) {
        return sr_lexer_next_alpha(lexer);
    }

    return sr_unicode_eof;
}

sr_inline_static sr_unicode_t sr_lexer_peek_alpha(sr_lexer_t *const lexer) {
    if (lexer->end >= sr_len(lexer->source)) {
        lexer->width = 0;
        return sr_unicode_eof;
    }
    return sr_string_code_at(lexer->source, lexer->end);
}

sr_inline_static sr_unicode_t sr_lexer_next_alpha(sr_lexer_t *const lexer) {
    sr_unicode_t alpha = sr_lexer_peek_alpha(lexer);
    if (alpha == sr_unicode_eof) {
        return sr_unicode_eof;
    }

    lexer->width = sr_unicode_width(alpha, lexer->source->encode_type);
    lexer->end += lexer->width;

    if (alpha == '\n') {
        lexer->local_pos.line++;
        lexer->local_pos.off = 0;
    }
    else {
        lexer->local_pos.off++;
    }

    return alpha;
}

sr_inline_static void sr_lexer_ignore(sr_lexer_t *const lexer) {
    lexer->start = lexer->end;
    lexer->start_pos = lexer->local_pos;
}

sr_inline_static sr_string_t *sr_lexer_word(sr_lexer_t *const lexer) {
    return sr_string_slice(lexer->source, lexer->start, lexer->end);
}
