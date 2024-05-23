#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

INLINE i32 is_alphabet(char c) {
    switch (c) {
    case 65: case 66: case 67: case 68: case 69:
    case 70: case 71: case 72: case 73: case 74:
    case 75: case 76: case 77: case 78: case 79:
    case 80: case 81: case 82: case 83: case 84:
    case 85: case 86: case 87: case 88: case 89:
    case 90: case 97: case 98: case 99: case 100:
    case 101: case 102: case 103: case 104: case 105:
    case 106: case 107: case 108: case 109: case 110:
    case 111: case 112: case 113: case 114: case 115:
    case 116: case 117: case 118: case 119: case 120:
    case 121: case 122: return 1;
    default: return 0;
    }
}

INLINE i32 is_number(char c) {
    switch (c) {
    case '0': case '1': case '2':
    case '3': case '4': case '5':
    case '6': case '7': case '8':
    case '9': return 1;
    default: return 0;
    }
}

static INLINE i32 is_string_literal_begin(lexer* lexer, char c) { return lexer->token_sets[TokenStringBegin].set_name[0][0] == c; }

static i32 is_operator_begin(lexer* lexer, char c) {
    for (u64 i = 0; i < lexer->token_sets[TokenOperator].set_size; ++i) {
        if (lexer->token_sets[TokenOperator].set_name[i][0] == c) {
            return 1;
        }
    }
    return 0;
}

static i32 is_separator_begin(lexer* lexer, char c) {
    for (u64 i = 0; i < lexer->token_sets[TokenSeparator].set_size; ++i) {
        if (lexer->token_sets[TokenSeparator].set_name[i][0] == c) {
            return 1;
        }
    }
    return 0;
}

void lexer_add_token(lexer* lexer, token_set set, Token token) {
    assert(lexer != NULL);
    assert(token >= 0 && token < TokenCount);
    lexer->token_sets[token] = set;
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

static i32 get_word_stride_test(const char* str) {
    for (u64 i = 0; str[i++] != 0;) {
        if (str[i] == 0)
            return i + 1;
        if (str[i] == '\n' || (str[i] != '_' && !is_alphabet(str[i]) && !is_number(str[i]))) {
            return i + 1;
        }
    }
    return 2;
}

static i32 is_number_vaild_prefix(const char* str) {
    switch (str[0]) {
    case '0': {
        switch (str[1]) {
        case 'b': case 'o': case 'd': case 'x':
            return 1;
        default:
            return 0;
        }
    }
    default:
        return 0;
    }
}

INLINE static i32 is_hex(char c) {
    switch (c) {
    case 97: case 98: case 99:
    case 100: case 101: case 102:
        return 1;
    default:
        return 0;
    }
}

static i32 get_number_literal_stride_test(const char* str) {
    i32 comma_count = 0;
    u64 i = 0;
    if (is_number_vaild_prefix(str)) {
        i = 2;
    }
    for (; str[i++] != 0;) {
        if (str[i] == 0)
            return i + 1;
        if (str[i] == '.') {
            if (++comma_count == 0) {
                return i + 1;
            }
            continue;
        }

        // default number literal separator
        if (!(is_hex(str[i]) || is_number(str[i])) || str[i] == ' ' || str[i] == '\n') {
            return i + 1;
        }
    }
    return 2;
}

static i32 get_string_literal_stride_test(lexer* lexer, const char* str) {
    u64 i = 0;
    while (!is_string_literal_begin(lexer, str[++i])) {
        if (str[i] == 0) {
            return -1;
        }
    }
    return i + 2;
}

static i32 get_token_type_location(lexer* lexer, Token type, const char* str, u64 len) {
    for (u64 i = 0; i < lexer->token_sets[type].set_size; ++i) {
        if (len != strlen(lexer->token_sets[type].set_name[i]))
            continue;
        if (strncmp(lexer->token_sets[type].set_name[i], str, len) == 0) {
            return i;
        }
    }
    return -1;
}

static token get_word_token(lexer* lexer, const char* str) {
    token result = { .name = str, .name_len = get_word_stride_test(str) - 1, .type = TokenKeyword, .name_location = -1 };
    if (result.name_len > -1) {
        result.name_location = get_token_type_location(lexer, TokenKeyword, str, result.name_len);
        if (result.name_location == -1)
            result.type = TokenIdentifier;
        return result;
    }
    return (token){ .type = TokenError };
}

INLINE static token get_literal_token(const char* str) {
    return (token){ .name = str, .name_len = get_number_literal_stride_test(str) - 1, .type = TokenLiteral, .name_location = -1 };
}

INLINE static token get_string_literal_token(lexer* lexer, const char* str) {
    return (token){ .name = str, .name_len = get_string_literal_stride_test(lexer, str) - 1, .type = TokenStringLiteral, .name_location = -1 };
}

static token get_operator_token(lexer* lexer, const char* str) {
    token tok = { .type = TokenOperator, .name = str, .name_len = 0 };
    for (u64 i = 0; i < lexer->token_sets[TokenOperator].set_size; ++i) {
        u64 operator_len = strlen(lexer->token_sets[TokenOperator].set_name[i]);
        if ((u64)tok.name_len < operator_len &&
            strncmp(tok.name, lexer->token_sets[TokenOperator].set_name[i], operator_len) == 0) {
            tok.name_len = operator_len;
            tok.name_location = i;
        }
    }
    return tok.name_location > -1 ? tok : (token){ .type = TokenError };
}

static token get_separator_token(lexer* lexer, const char* str) {
    token tok = { .type = TokenSeparator, .name = str, .name_len = 0 };
    for (u64 i = 0; i < lexer->token_sets[TokenSeparator].set_size; ++i) {
        u64 operator_len = strlen(lexer->token_sets[TokenSeparator].set_name[i]);
        if ((u64)tok.name_len < operator_len && 
            strncmp(str, lexer->token_sets[TokenSeparator].set_name[i], operator_len) == 0) {
            tok.name_len = operator_len;
            tok.name_location = i;
        }
    }
    return tok.name_location > -1 ? tok : (token){ .type = TokenError };
}

token get_token_test(lexer* lexer, const char* str) {
    if (str[0] == 0)
        return (token){ .type = TokenError };
    if (is_alphabet(str[0]) || str[0] == '_') {
        return get_word_token(lexer, str);
    }
    if (is_number(str[0])) {
        return get_literal_token(str);
    }
    if (is_string_literal_begin(lexer, str[0])) {
        return get_string_literal_token(lexer, str);
    }
    if (is_operator_begin(lexer, str[0])) {
        return get_operator_token(lexer, str);
    }
    if (is_separator_begin(lexer, str[0])) {
        return get_separator_token(lexer, str);
    }
    return (token){ .type = TokenError };
}

token lexer_tokenize_string(lexer* lexer, const char* str) {
    assert(lexer != NULL);

    token tok = { .type = TokenError };
    if (is_separator_begin(lexer, str[0])) {
        tok = get_separator_token(lexer, str);
    }
    if (tok.type == TokenError) {
        switch (str[0]) {
            case '{': return (token){ .type = TokenOpenBrace, .name_len = 1, .name = "{" };
            case '}': return (token){ .type = TokenCloseBrace, .name_len = 1, .name = "}" };
            case '(': return (token){ .type = TokenOpenSquareBracket, .name_len = 1, .name = "(" };
            case ')': return (token){ .type = TokenCloseSquareBracket, .name_len = 1, .name = ")" };
            case '[': return (token){ .type = TokenOpenRoundBracket, .name_len = 1, .name = "[" };
            case ']': return (token){ .type = TokenCloseRoundBracket, .name_len = 1, .name = "]" };
            case ';': return (token){ .type = TokenSemicolon, .name_len = 1, .name = ";" };
            case ',': return (token){ .type = TokenComma, .name_len = 1, .name = "," };
            case '.': return (token){ .type = TokenFullStop, .name_len = 1, .name = "." };
            case '\n': return (token){ .type = TokenNewLine, .name_len = 1, .name = "\n" };
            default: break;
        }
    }

    tok = get_token_test(lexer, str);
    if (tok.name_len < 0) {
        tok.type = TokenError;
    }

    return tok;
}

vector(token) lexer_tokenize_until(lexer* lexer, const char* str, const char terminal) {
    vector(token) tokens = make_vector();
    const char* begin = str;
    token tok = { .name = "" };
    while (tok.name[0] != terminal) {
        if (begin[0] == ' ') {
            begin++;
            continue;
        }
        if (begin[0] == 0) {
            vector_pushe(tokens, (token){ .type = TokenEnd, .name_len = -1 });
            return tokens;
        }
        tok = lexer_tokenize_string(lexer, begin);
        vector_pushe(tokens, tok);
        if (tok.type == TokenError) {
            return tokens;
        }
        begin += tok.name_len;
    }

    return tokens;
}

