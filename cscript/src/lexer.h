#ifndef LEXER_H
#define LEXER_H
#include "basic/vector.h"

typedef enum {
    TokenKeyword = 0,
    TokenSeparator,
    TokenOperator,
    TokenParserCount,

    TokenCharLiteral,
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

typedef struct {
    i16 sub_type, name_len, line;
    Token type;
    char* name;
    union {
        u8 _char;
        i64 _int;
        f64 _float;
    } val;
} token;

typedef struct {
    i16 set_size;
    Token token;
    const char** set_name;
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
i32 is_char_literal(token* tok);
i32 is_real_number(token* tok);

void lexer_add_token(lexer* lexer, token_set set, Token token);
token lexer_tokenize_string(lexer* lexer, char* str);
vector(token) lexer_tokenize_until(lexer* lexer, char* str, char terminal);

void print_token_name(token* tok);
void print_token(token* tok);

#endif
