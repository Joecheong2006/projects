#ifndef LEXER_H
#define LEXER_H
#include "basic/vector.h"

typedef enum {
    TokenKeyword = 0,
    TokenSeparator,
    TokenOperator,
    TokenParserCount,

    TokenStringBegin,
    TokenStringLiteral,
    TokenDecLiteral,
    TokenHexLiteral,
    TokenOctLiteral,
    TokenBinLiteral,
    TokenIdentifier,
    TokenCount,

    TokenOpenBrace,
    TokenCloseBrace,
    TokenOpenSquareBracket,
    TokenCloseSquareBracket,
    TokenOpenRoundBracket,
    TokenCloseRoundBracket,
    TokenSemicolon,
    TokenComma,
    TokenFullStop,
    TokenNewLine,

    TokenEnd,
    TokenError,
    TokenUnkown
} Token;

typedef struct PACKED {
    Token type;
    char* name;
    i16 sub_type;
    i16 name_len;
} token;

typedef struct PACKED {
    const char** set_name;
    u64 set_size;
    Token token;
} token_set;

typedef struct {
    token_set token_sets[TokenCount];
} lexer;

i32 is_alphabet(char c);
i32 is_number(char c);
i32 is_identifier(token* tok);
i32 is_keyword(token* tok, i32 type);
i32 is_operator(token* tok);
i32 is_separator(token* tok);
i32 is_keyword_type(token* tok, i32 type);
i32 is_operator_type(token* tok, i32 type);
i32 is_separator_type(token* tok, i32 type);
i32 is_string_literal(token* tok);
i32 is_real_number(token* tok);

void lexer_add_token(lexer* lexer, token_set set, Token token);
i32 compare_strings(const char** strings, u64 strings_len, const char* str);
i32 compare_token_set(token_set* token_set, const char* str);

token lexer_tokenize_string(lexer* lexer, char* str);
vector(token) lexer_tokenize_until(lexer* lexer, char* str, char terminal);

void print_token_name(token* tok);
void print_token(token* tok);

#endif
