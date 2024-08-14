#ifndef _LEXER_H_
#define _LEXER_H_
#include "container/string.h"

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

typedef union {
    i32 int32;
    f32 float32;
    const char* string;
    i32 type[3];
} primitive_data;

typedef struct {
    primitive_data val;
    i32 line, count;
    TokenType type;
} token;

typedef struct {
    const char* ctx;
    i32 ctx_len;
    i32 line, position, str_count;
} lexer;

void lexer_load_file_text(lexer* lex, const char* file_name);
vector(token) generate_tokens(lexer* lex);

#endif
