#include "lexer.h"

#include "keys_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static i64 base_n_to_dec(token* node, int base_n, i64(*is_digit)(const char c)) {
    int result = 0;
    const char* str = node->name + 2;
    int len = node->name_len - 2;
    for (; str[0] == '0'; str++, len--) {}

    for (i32 i = 0; i < len; ++i) {
        result += powl(base_n, i) * is_digit(str[len - i - 1]);
    }
    return result;
}

static INLINE i64 hex_to_dec_ch(const char c) {
    return c >= 'a' && c <= 'f' ? c - 'W' : c - '0';
}

static INLINE i64 hex_to_dec(token* tok) {
    return base_n_to_dec(tok, 16, hex_to_dec_ch);
}

static INLINE i64 bin_to_dec_ch(const char c) {
    return c == '1';
}

static INLINE i64 bin_to_dec(token* tok) {
    return base_n_to_dec(tok, 2, bin_to_dec_ch);
}

static INLINE i64 oct_to_dec_ch(const char c) {
    return c - '0';
}

static INLINE i64 oct_to_dec(token* tok) {
    return base_n_to_dec(tok, 8, oct_to_dec_ch);
}


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

INLINE i32 is_identifier(token* tok) { return tok->type == TokenIdentifier; }
INLINE i32 is_keyword(token* tok, i32 type) { return tok->sub_type == type; }
INLINE i32 is_operator(token* tok) { return tok->type == TokenOperator; }
INLINE i32 is_separator(token* tok) { return tok->type == TokenSeparator; }

INLINE i32 is_keyword_type(token* tok, i32 type) { return tok->type == TokenKeyword && tok->sub_type == type; }
INLINE i32 is_operator_type(token* tok, i32 type) { return tok->type == TokenOperator && tok->sub_type == type; }
INLINE i32 is_separator_type(token* tok, i32 type) { return tok->type == TokenSeparator && tok->sub_type == type; }
INLINE i32 is_string_literal(token* tok) { return tok->type == TokenStringLiteral; }
INLINE i32 is_char_literal(token* tok) { return tok->type == TokenCharLiteral; }
i32 is_real_number(token* tok) { 
    // if (is_number(tok->name[0] || (tok->name[0] == '.' && is_number(tok->name[1])))) {
    //     return 1;
    // }

    for (i32 i = 0; i < tok->name_len; ++i)
        if (tok->name[i] == '.')
            return 1;
    return 0;
}

static INLINE i32 is_string_literal_begin(char c) { return c == '\'' || c == '"'; }

static i32 is_separator_begin(lexer* lexer, char c) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    for (i32 i = 0; i < lexer->token_sets[TokenSeparator].set_size; ++i) {
        if (lexer->token_sets[TokenSeparator].set_name[i][0] == c) {
            return 1;
        }
    }
    return 0;
}

void lexer_add_token(lexer* lexer, token_set set, Token token) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    ASSERT_MSG(token >= 0 && token < TokenCount, "adding invalid token");
    lexer->token_sets[token] = set;
}

static i32 get_word_stride(const char* str) {
    for (u64 i = 0; str[i++] != 0;) {
        if (str[i] == 0)
            return i + 1;
        if (str[i] != '_' && !is_alphabet(str[i]) && !is_number(str[i])) {
            return i + 1;
        }
    }
    return 2;
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

INLINE static i32 is_oct(char c) {
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': return 1;
    default: return 0;
    }
}

INLINE static i32 is_bin(char c) {
    switch (c) {
    case '0':
    case '1': return 1;
    default: return 0;
    }
}

static token get_bin_literal_stride(char* str) {
    token result = { .name = str, .type = TokenError, .sub_type = -1, .name_len = -1 };
    i32 i = 2;
    if (!is_bin(str[i])) {
        return result;
    }
    while (1) {
        if (!is_bin(str[i])) {
            break;
        }
        ++i;
    }
    result.type = TokenBinLiteral;
    result.name_len = i;
    result.val._int = bin_to_dec(&result);
    return result;
}

static token get_oct_literal_stride(char* str) {
    token result = { .name = str, .type = TokenError, .sub_type = -1, .name_len = -1 };
    i32 i = 2;
    if (!is_oct(str[i])) {
        return result;
    }
    while (1) {
        if (!is_oct(str[i])) {
            break;
        }
        ++i;
    }
    result.type = TokenOctLiteral;
    result.name_len = i;
    result.val._int = oct_to_dec(&result);
    return result;
}

static token get_hex_literal_stride(char* str) {
    token result = { .name = str, .type = TokenError, .sub_type = -1, .name_len = -1 };
    i32 i = 2;
    if (!(is_number(str[i]) || is_hex(str[i]))) {
        return result;
    }
    while (1) {
        if (!(is_number(str[i]) || is_hex(str[i]))) {
            break;
        }
        ++i;
    }
    result.type = TokenHexLiteral;
    result.name_len = i;
    result.val._int = hex_to_dec(&result);
    return result;
}

static token get_dec_literal_token(char* str) {
    token result = { .name = str, .type = TokenError, .sub_type = -1, .name_len = -1 };
    i32 i = 0;
    if (!(is_number(str[i]))) {
        return result;
    }
    i32 comma_count = 0;
    while (1) {
        if (str[i] == '.') {
            if (comma_count++ == 0) {
                i++;
                continue;
            }
            return result;
        }
        if (!(is_number(str[i]))) {
            break;
        }
        ++i;
    }

    result.type = TokenDecLiteral;
    result.name_len = i;

    if (comma_count > 0) {
        result.val._float = atof(str);
    }
    else {
        result.val._int = atoi(str);
    }

    return result;
}

static i32 get_string_literal_stride(char* str) {
    i32 i = 0;
    i32 len = 0;
    while (!is_string_literal_begin(str[++i])) {
        ++len;
        if (str[i] == '\\') {
            ++i;
            switch (str[i]) {
            case 'a': str[i] = '\a'; break;
            case 'b': str[i] = '\b'; break;
            case 'f': str[i] = '\f'; break;
            case 'n': str[i] = '\n'; break;
            case 'r': str[i] = '\r'; break;
            case 't': str[i] = '\t'; break;
            case 'v': str[i] = '\v'; break;
            case '\\': str[i] = '\\'; break;
            case '\'': str[i] = '\''; break;
            case '\"': str[i] = '\"'; break;
            case '\?': str[i] = '\?'; break;
            default: return -1;
            }
            continue;
        }
        if (str[i] == 0) {
            return -1;
        }
    }
    return len + 3;
}

INLINE static token get_string_literal_token(char* str) {
    token result = { .name = str + 1, .name_len = get_string_literal_stride(str) - 1, .type = TokenStringLiteral, .sub_type = -1 };
    if (result.name_len < 0) {
        result.type = TokenError;
        return result;
    }
    if (result.name_len == 3) {
        result.type = TokenCharLiteral;
    }
    return result;
}

static i32 get_token_type_location(lexer* lexer, Token type, const char* str, u64 len) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    for (i32 i = 0; i < lexer->token_sets[type].set_size; ++i) {
        if (len != strlen(lexer->token_sets[type].set_name[i]))
            continue;
        if (strncmp(lexer->token_sets[type].set_name[i], str, len) == 0) {
            return i;
        }
    }
    return -1;
}

static token get_word_token(lexer* lexer, char* str) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    token result = { .name = str, .name_len = get_word_stride(str) - 1, .type = TokenKeyword, .sub_type = -1 };
    if (result.name_len > 0) {
        result.sub_type = get_token_type_location(lexer, TokenKeyword, str, result.name_len);
        if (result.sub_type == -1)
            result.type = TokenIdentifier;
        return result;
    }
    return (token){ .type = TokenError };
}

INLINE static token get_literal_token(char* str) {
    switch (str[0]) {
    case '0': {
        switch (str[1]) {
        case 'b': return get_bin_literal_stride(str);
        case 'o': return get_oct_literal_stride(str);
        case 'x': return get_hex_literal_stride(str);
        default: return get_dec_literal_token(str);
        }
    }
    default: return get_dec_literal_token(str);
    }
}

static token get_operator_token(lexer* lexer, char* str) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    token tok = { tok.sub_type = -1, .name_len = 0, .type = TokenOperator, .name = str };
    for (i32 i = 0; i < lexer->token_sets[TokenOperator].set_size; ++i) {
        i32 operator_len = strlen(lexer->token_sets[TokenOperator].set_name[i]);
        if (tok.name_len < operator_len &&
            strncmp(tok.name, lexer->token_sets[TokenOperator].set_name[i], operator_len) == 0) {
            tok.name_len = operator_len;
            tok.sub_type = i;
        }
    }
    return tok.sub_type > -1 ? tok : (token){ .type = TokenError };
}

static token get_separator_token(lexer* lexer, char* str) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    token tok = { tok.sub_type = -1, .name_len = 0, .type = TokenSeparator, .name = str };
    for (i32 i = 0; i < lexer->token_sets[TokenSeparator].set_size; ++i) {
        i32 operator_len = strlen(lexer->token_sets[TokenSeparator].set_name[i]);
        if (tok.name_len < operator_len && 
            strncmp(str, lexer->token_sets[TokenSeparator].set_name[i], operator_len) == 0) {
            tok.name_len = operator_len;
            tok.sub_type = i;
        }
    }
    return tok.sub_type > -1 ? tok : (token){ .type = TokenError };
}

token lexer_tokenize_string(lexer* lexer, char* str) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");

    token tok = { .type = TokenError };
    if (is_separator_begin(lexer, str[0])) {
        tok = get_separator_token(lexer, str);
    }
    if (tok.type != TokenError)
        return tok;

    switch (str[0]) {
        case '{': return (token){ .type = TokenOpenBrace, .name_len = 1, .name = "{" };
        case '}': return (token){ .type = TokenCloseBrace, .name_len = 1, .name = "}" };
        case '(': return (token){ .type = TokenOpenRoundBracket, .name_len = 1, .name = "(" };
        case ')': return (token){ .type = TokenCloseRoundBracket, .name_len = 1, .name = ")" };
        case '[': return (token){ .type = TokenOpenSquareBracket, .name_len = 1, .name = "[" };
        case ']': return (token){ .type = TokenCloseSquareBracket, .name_len = 1, .name = "]" };
        case ';': return (token){ .type = TokenSemicolon, .name_len = 1, .name = ";" };
        case ',': return (token){ .type = TokenComma, .name_len = 1, .name = "," };
        case '.': return (token){ .type = TokenFullStop, .name_len = 1, .name = "." };
        case '\n': return (token){ .type = TokenNewLine, .name_len = 1, .name = "\n" };
        case '\'': case '"': return get_string_literal_token(str);
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
        case 121: case 122: case '_': return get_word_token(lexer, str);
        case '0': case '1': case '2':
        case '3': case '4': case '5':
        case '6': case '7': case '8':
        case '9': return get_literal_token(str);
        default: return get_operator_token(lexer, str);
    }
}

vector(token) lexer_tokenize_until(lexer* lexer, char* str, char terminal) {
    ASSERT_MSG(lexer != NULL, "invalid lexer");
    vector(token) tokens = make_vector();

    if (str[0] == 0) {
        vector_pushe(tokens, (token){
                .type = TokenEnd
                });
        return tokens;
    }

    char* begin = str;
    token tok = { .name = "" };

    while (tok.name[0] != terminal) {
        if (begin[0] == 0) {
            vector_pushe(tokens, (token){ .type = TokenEnd, .name_len = -1 });
            return tokens;
        }
        if (begin[0] == ' ') {
            begin++;
            continue;
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

void print_token_name(token* tok) {
    i32 n = 0, len = tok->name_len;
    if (is_string_literal(tok)) {
        len -= 2;
    }

    for (; n < len; ++n) {
        if (tok->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(tok->name[n]);
    }
}

static void _print_token(token* token, const char* type_name, const char** type_names) {
    printf("(%s, '", type_name);
    for (i32 n = 0; n < token->name_len; ++n) {
        if (token->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(token->name[n]);
    }
    (void)(type_names);
    printf("', %d)\n", token->name_len);
}

void print_token(token* tok) {
    switch (tok->type) {
    case TokenOpenBrace:
    case TokenCloseBrace:
    case TokenOpenSquareBracket:
    case TokenCloseSquareBracket:
    case TokenOpenRoundBracket:
    case TokenCloseRoundBracket:
    case TokenSemicolon:
    case TokenComma:
    case TokenFullStop:
    case TokenNewLine:
                   _print_token(tok, "default separator", NULL); break;
    case TokenKeyword: _print_token(tok, "keyword", Keyword); break;
    case TokenSeparator: _print_token(tok, "separator", Separator); break;
    case TokenOperator: _print_token(tok, "operator", Operator); break;
    case TokenDecLiteral: _print_token(tok, "dec", NULL); break;
    case TokenHexLiteral: _print_token(tok, "hex", NULL); break;
    case TokenOctLiteral: _print_token(tok, "oct", NULL); break;
    case TokenBinLiteral: _print_token(tok, "string", NULL); break;
    case TokenCharLiteral:
    case TokenStringLiteral: {
        token temp = *tok;
        temp.name += 1;
        temp.name_len -= 2;
        _print_token(&temp, "string literal", NULL);
    } break;
    case TokenIdentifier: _print_token(tok, "identifier", NULL); break;
    case TokenEnd: _print_token(tok, "end", NULL); break;
    case TokenError: _print_token(tok, "error", NULL); break;
    default: _print_token(tok, "unkown", NULL); break;
    }
}

void print_name(char* str, i32 str_len) {
    for (i32 n = 0; n < str_len; ++n) {
        if (str[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(str[n]);
    }
}

