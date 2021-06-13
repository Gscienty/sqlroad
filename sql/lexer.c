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
sr_inline_static void sr_lexer_prev_alpha(sr_lexer_t *const lexer);
sr_inline_static sr_unicode_t sr_lexer_peek_alpha(sr_lexer_t *const lexer);
sr_inline_static void sr_lexer_ignore(sr_lexer_t *const lexer);
sr_inline_static sr_string_t *sr_lexer_word(sr_lexer_t *const lexer);
sr_inline_static void sr_lexer_product(sr_lexer_t *const lexer, sr_string_t *word, sr_token_type_t type);
static _Bool sr_lexer_accept(sr_lexer_t *const lexer, const sr_unicode_t codes[]);
static sr_lexer_fn_t sr_lexer_process(sr_lexer_t *const lexer);
static sr_lexer_fn_t sr_lexer_process_eof(sr_lexer_t *const lexer);
static sr_lexer_fn_t sr_lexer_process_number(sr_lexer_t *const lexer);
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
        goto next_loop;
    }
    if (alpha == '\'' || alpha == '"') {
        if ((lexer->error = sr_lexer_scan_string(lexer, alpha)) != 0) {
            return (sr_lexer_fn_t) { .fn = NULL };
        }

        sr_lexer_product(lexer, sr_lexer_word(lexer), sr_token_string);
        goto next_loop;
    }
    if ('0' <= alpha && alpha <= '9') {
        sr_lexer_prev_alpha(lexer);
        return (sr_lexer_fn_t) { .fn = sr_lexer_process_number };
    }

next_loop:
    return (sr_lexer_fn_t) { .fn = sr_lexer_process };
}

static sr_lexer_fn_t sr_lexer_process_eof(sr_lexer_t *const lexer) {
    sr_token_t *token = sr_malloc(sizeof(sr_token_t));
    sr_linker_init(token);

    token->token = NULL;
    token->type = sr_token_eof;

    sr_link_insert_before(lexer->tokens, token);

    return (sr_lexer_fn_t) { .fn = NULL };
}

static sr_lexer_fn_t sr_lexer_process_number(sr_lexer_t *const lexer) {
    static const sr_unicode_t bin_digest[] = { '0', '1', sr_unicode_eof };
    static const sr_unicode_t oct_digest[] = { '0', '1', '2', '3', '4', '5', '6', '7', sr_unicode_eof };
    static const sr_unicode_t dec_digest[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', sr_unicode_eof };
    static const sr_unicode_t hex_digest[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F', sr_unicode_eof };

    const sr_unicode_t *digest = dec_digest;
    if (sr_lexer_accept(lexer, (sr_unicode_t []) { '0', sr_unicode_eof })) {
        if (sr_lexer_accept(lexer, (sr_unicode_t []) { 'o', 'O', sr_unicode_eof })) {
            digest = oct_digest;
        }
        else if (sr_lexer_accept(lexer, (sr_unicode_t []) { 'x', 'X', sr_unicode_eof })) {
            digest = hex_digest;
        }
        else if (sr_lexer_accept(lexer, (sr_unicode_t []) { 'b', 'B', sr_unicode_eof })) {
            digest = bin_digest;
        }
    }

    _Bool next;
    for (next = sr_lexer_accept(lexer, digest); next; next = sr_lexer_accept(lexer, digest));

    if (sr_lexer_accept(lexer, (sr_unicode_t []) { '.', sr_unicode_eof })) {
        for (next = sr_lexer_accept(lexer, digest); next; next = sr_lexer_accept(lexer, digest));
    }

    return (sr_lexer_fn_t) { .fn = sr_lexer_process };
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

static _Bool sr_lexer_accept(sr_lexer_t *const lexer, const sr_unicode_t codes[]) {
    sr_unicode_t alpha = sr_lexer_peek_alpha(lexer);

    int i;
    for (i = 0; codes[i] != sr_unicode_eof; i++) {
        if (alpha == codes[i]) {
            sr_lexer_next_alpha(lexer);
            return 1;
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

sr_inline_static void sr_lexer_prev_alpha(sr_lexer_t *const lexer) {
    lexer->end -= lexer->width;
    lexer->local_pos = lexer->prev_pos;
}

sr_inline_static void sr_lexer_ignore(sr_lexer_t *const lexer) {
    lexer->start = lexer->end;
    lexer->start_pos = lexer->local_pos;
}

sr_inline_static sr_string_t *sr_lexer_word(sr_lexer_t *const lexer) {
    return sr_string_slice(lexer->source, lexer->start, lexer->end);
}

sr_inline_static void sr_lexer_product(sr_lexer_t *const lexer, sr_string_t *word, sr_token_type_t type) {
    sr_token_t *token = sr_malloc(sizeof(sr_token_t));
    sr_linker_init(token);
    token->type = type;
    token->token = word;

    sr_link_insert_before(lexer->tokens, token);

    lexer->start = lexer->end;
    lexer->start_pos = lexer->local_pos;
}
