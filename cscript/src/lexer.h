#ifndef LEXER_H
#define LEXER_H

#include "vector.h"

typedef enum {
    TokenKeyword = 0,
    TokenSeparator,
    TokenOperator,
    TokenParserCount,

    TokenStringBegin,
    TokenStringLiteral,
    TokenLiteral,
    TokenIdentifier,
    TokenCount,

    TokenOpenBrace,
    TokenCloseBrace,
    TokenOpenSquareBracket,
    TokenCloseSquareBracket,
    TokenOpenRoundBracket,
    TokenCloseRoundBracket,
    TokenSemicolon, TokenComma,
    TokenFullStop,
    TokenNewLine,

    TokenEnd,
    TokenError,
    TokenUnkown
} Token;

typedef struct {
    const char* name;
    i32 name_len;
    i32 name_location;
    Token type;
} token;

typedef struct {
    const char** set_name;
    u64 set_size;
    Token token;
} token_set;

typedef struct {
    token_set token_sets[TokenCount];
} lexer;

i32 is_alphabet(char c);
i32 is_number(char c);
void lexer_add_token(lexer* lexer, token_set set, Token token);
i32 compare_strings(const char** strings, u64 strings_len, const char* str);
i32 compare_token_set(token_set* token_set, const char* str);

token lexer_tokenize_string(lexer* lexer, const char* str);
vector(token) lexer_tokenize_until(lexer* lexer, const char* str, const char terminal);

#endif
