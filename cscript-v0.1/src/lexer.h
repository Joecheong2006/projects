#ifndef _LEXER_H_
#define _LEXER_H_
#include "container/vector.h"
#include "primitive_data.h"

typedef enum {
    TokenTypeIdentifier = 256,
    TokenTypeComment,

    TokenTypeOperatorEqual,
    TokenTypeOperatorNotEqual,
    TokenTypeOperatorGreaterThan,
    TokenTypeOperatorLessThan,

    TokenTypeKeywordVar,
    TokenTypeKeywordFun,
    TokenTypeKeywordIf,
    TokenTypeKeywordElif,
    TokenTypeKeywordElse,
    TokenTypeKeywordOr,
    TokenTypeKeywordAnd,
    TokenTypeKeywordWhile,
    TokenTypeKeywordDo,
    TokenTypeKeywordFor,

    TokenTypeLiteralString,
    TokenTypeLiteralInt32,
    TokenTypeLiteralFloat32,

    TokenTypeEOF,
} TokenType;
extern const char* TokenTypeString[TokenTypeEOF - 255];

struct token {
    primitive_data val;
    i32 line, count;
    TokenType type;
};

typedef struct token token;

typedef struct {
    token* tok;
    const char* msg;
} error_info;

typedef struct {
    const char* ctx;
    i32 ctx_len;
    i32 line, position, str_count;
} lexer;

void lexer_load_file_text(lexer* lex, const char* file_name);
vector(token) generate_tokens(lexer* lex);

#endif
