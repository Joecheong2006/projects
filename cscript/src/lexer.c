#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

INLINE i32 is_alphabet(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
INLINE i32 is_number(char c) { return c >= '0' && c <= '9'; }
INLINE i32 is_string_literal_begin(lexer* lexer, char c) { return lexer->token_sets[TokenStringBegin].set_name[0][0] == c; }

void lexer_add_token(lexer* lexer, token_set set, Token token) {
    assert(lexer != NULL);
    assert(token >= 0 && token < TokenCount);
    lexer->token_sets[token] = set;
}

i32 match_token(token_set* token_set, const char* str) {
    for (u64 i = 0; i < token_set->set_size; ++i) {
        char* cp = strchr(str, token_set->set_name[i][0]);
        if (cp != NULL) {
            u64 len = strlen(token_set->set_name[i]);
            if (strncmp(token_set->set_name[i], cp, len) == 0) {
                return cp - str + 1;
            }
        }
    }
    return -1;
}

i32 compare_strings(const char** strings, u64 strings_len, const char* str) {
    for (u64 i = 0; i < strings_len; ++i) {
        if (strncmp(str, strings[i], strlen(strings[i])) == 0) {
            return 1;
        }
    }
    return 0;
}

INLINE i32 compare_token_set(token_set* token_set, const char* str) {
    return compare_strings(token_set->set_name, token_set->set_size, str);
}

token compare_with_token_sets(lexer* lexer, const char* str) {
    for (u64 i = 0; i < TokenParserCount; ++i) {
        for (u64 j = 0; j < lexer->token_sets[i].set_size; ++j) {
            u64 len = strlen(lexer->token_sets[i].set_name[j]);
            if (strncmp(lexer->token_sets[i].set_name[j], str, len) == 0) {
                return (token){
                    .name = str,
                    .name_len = len,
                    .name_location = j,
                    .type = lexer->token_sets[i].token,
                };
            }
        }
    }
    return (token){ .name = str, .name_len = -1, .type = -1, .name_location = -1 };
}

i32 get_token_stride(lexer* lexer, const char* str) {
    if (str[0] == 0)
        return -1;

    if (!is_alphabet(str[0])) {
        for (u64 j = 0; j < lexer->token_sets[TokenSeparator].set_size; ++j) {
            if (str[0] == lexer->token_sets[TokenSeparator].set_name[j][0]) {
                return 2;
            }
        }
    }

    if (is_alphabet(str[0])) {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (!is_alphabet(str[i]) && !is_number(str[i])) {
                return i + 1;
            }
        }
    }
    else if (is_number(str[0])) {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (str[i] == '.') {
                continue;
            }
            // default number literal separator
            if (str[i] == ' ' || str[i] == '\n' || 
                compare_token_set(&lexer->token_sets[TokenOperator], str + i) ||
                compare_token_set(&lexer->token_sets[TokenSeparator], str + i)) {
                return i + 1;
            }
        }
    }
    else if (is_string_literal_begin(lexer, str[0])) {
        u64 i = 0;
        while (!is_string_literal_begin(lexer, str[++i])) {
            if (str[i] == '\0') {
                return -1;
            }
        }
        return i;
    }
    else {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (str[i] == ' ' || is_alphabet(str[i]) || is_number(str[i])) {
                return i + 1;
            }
            if (compare_token_set(&lexer->token_sets[TokenSeparator], str + i)) {
                return i + 1;
            }
            if (compare_token_set(&lexer->token_sets[TokenStringBegin], str + i)) {
                return i + 1;
            }
        }
    }

    return -1;
}

vector(token) lexer_tokenize_str(lexer* lexer, const char* str, u64 str_size) {
    assert(lexer != NULL);
    u64 str_begin_offset = 0;
    i32 nword_len = 0;

    vector(token) tokens = make_vector();

    while (str_begin_offset < str_size) {
        nword_len = get_token_stride(lexer, str + str_begin_offset) - 1;
        if (nword_len < 0) {
            return tokens;
        }

        if (*(str + str_begin_offset) == ' ') {
            str_begin_offset++;
            continue;
        }

        token result = compare_with_token_sets(lexer, str + str_begin_offset);
        str_begin_offset += nword_len;
        if (result.type >= 0) {
            vector_pushe(tokens, result);
            continue;
        }
        if (is_number(result.name[0])) {
            result.type = TokenLiteral;
        }
        else {
            result.type = TokenIdentifier;
        }
        result.name_len = nword_len;
        vector_pushe(tokens, result);
        // vector_push(tokens, str + str_begin_offset, nword_len, -1, -1);
    }

    return tokens;
}

vector(token) lexer_tokenize_until(lexer* lexer, const char* str, const char terminal) {
    vector(token) tokens = make_vector();
    const char* begin = str;
    token tok = { .name = "" };
    u64 str_len = strlen(str);
    while (tok.name[0] != terminal) {
        if (begin[0] == ' ') {
            begin++;
            continue;
        }
        tok = lexer_tokenize_string(lexer, begin);
        if (tok.name == NULL) {
            if ((u64)(begin - str) == str_len) {
                tok.type = TokenEnd;
            }
            vector_pushe(tokens, tok);
            break;
        }
        vector_pushe(tokens, tok);
        begin += tok.name_len;
    }

    vector_pop(tokens);

    return tokens;
}

static token ncompare_with_token_sets(lexer* lexer, const char* str, u64 len) {
    for (u64 i = 0; i < TokenParserCount; ++i) {
        for (u64 j = 0; j < lexer->token_sets[i].set_size; ++j) {
            if (len != strlen(lexer->token_sets[i].set_name[j]))
                continue;
            if (strncmp(lexer->token_sets[i].set_name[j], str, len) == 0) {
                return (token){
                    .name = str,
                    .name_len = len,
                    .name_location = j,
                    .type = lexer->token_sets[i].token,
                };
            }
        }
    }
    return (token){ .name = str, .name_len = -1, .type = TokenUnkown, .name_location = -1 };
}

token lexer_tokenize_string(lexer* lexer, const char* str) {
    assert(lexer != NULL);

    i32 nword_len = get_token_stride(lexer, str) - 1;
    if (nword_len < 0) {
        return (token){NULL, -1, -1, TokenError};
    }

    token result = ncompare_with_token_sets(lexer, str, nword_len);
    if (result.type != TokenUnkown) {
        return result;
    }

    if (is_number(result.name[0])) {
        result.type = TokenLiteral;
    }
    else if (is_string_literal_begin(lexer, result.name[0])) {
        result.type = TokenStringLiteral;
        nword_len += 2;
    }
    else {
        result.type = TokenIdentifier;
    }
    result.name_len = nword_len;
    return result;
}

