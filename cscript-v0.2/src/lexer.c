#include "lexer.h"
#include "core/log.h"
#include "core/assert.h"
#include "container/string.h"
#include <string.h>
#include <stdio.h>
#include "tracing.h"

const char* TokenTypeString[] = {
    [TokenTypeIdentifier - 256] = "",
    [TokenTypeComment - 256] = "",
 
    [TokenTypeOperatorEqual - 256] = "==",
    [TokenTypeOperatorNotEqual - 256] = "!=",
    [TokenTypeOperatorGreaterThan - 256] = ">=",
    [TokenTypeOperatorLessThan - 256] = "<=",
 
    [TokenTypeKeywordVar - 256] = "var",
    [TokenTypeKeywordFun - 256] = "func",
    [TokenTypeKeywordRet - 256] = "return",
    [TokenTypeKeywordIf - 256] = "if",
    [TokenTypeKeywordElif - 256] = "elif",
    [TokenTypeKeywordElse - 256] = "else",
    [TokenTypeKeywordOr - 256] = "or",
    [TokenTypeKeywordAnd - 256] = "and",
    [TokenTypeKeywordWhile - 256] = "while",
    [TokenTypeKeywordDo - 256] = "do",
    [TokenTypeKeywordEnd - 256] = "end",
    [TokenTypeKeywordFor - 256] = "for",

    [TokenTypeAssignmentPlus - 256] = "+=",
    [TokenTypeAssignmentMinus - 256] = "-=",
    [TokenTypeAssignmentMultiply - 256] = "*=",
    [TokenTypeAssignmentDivide - 256] = "/=",
    [TokenTypeAssignmentModulus - 256] = "%=",
 
    [TokenTypeLiteralString - 256] = "",
    [TokenTypeLiteralInt - 256] = "",
    [TokenTypeLiteralFloat - 256] = "",
 
    [TokenTypeEOF - 256] = "EOF",
};

static vector(char) load_file(const char* file_name, const char* mode) {
    FILE* file = fopen(file_name, mode);
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    vector(char) result = make_vector(char);
    vector_resize(result, size + 1);
    fread(result, 1, size, file);
    result[size] = 0;
    fclose(file);
    return result;
}

void lexer_load_file_text(lexer* lex, const char* file_name) {
    START_PROFILING();
    vector(char) ctx = load_file(file_name, "rb");
    lex->ctx_len = strlen(ctx);
    lex->ctx = ctx;
    END_PROFILING(__func__);
}

static void lexer_consume(lexer* lex) {
    ++lex->str_count;
    ++lex->position;
}

static void lexer_consume_n(lexer* lex, i32 n) {
    lex->str_count += n;
    lex->position += n;
}

static char lexer_get_consumed(lexer* lex) {
    lexer_consume(lex);
    return lex->ctx[lex->str_count];
}

static i32 is_0_9(char ch) {
    return ch >= '0' && ch <= '9';
}

static token generate_float_after_dot(lexer* lex) {
    i64 val = 0, count = 0, percision_count = 1;
    char c = lex->ctx[lex->str_count];
    while (c >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        percision_count *= 10;
        ++count;
        c = lex->ctx[++lex->str_count];
    }
    lex->position += count;
    token tok = { { .val.float64 = (f64)val / percision_count } , lex->line, lex->position - count, TokenTypeLiteralFloat };
    tok.data.type = PrimitiveDataTypeFloat64;
    return tok;
}

static token generate_number_literal_token(lexer* lex) {
    START_PROFILING();
    i64 val = 0, count = 0, percision_count = 0;
    char c = lex->ctx[lex->str_count];
    if (c == '0' && !is_0_9(lex->ctx[lex->str_count + 1]) && lex->ctx[lex->str_count + 1] != '.') {
        c = lexer_get_consumed(lex);
        switch (c) {
        case 'x': {
            c = lexer_get_consumed(lex);
            while ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || ((c >= 'A' && c <= 'F'))) {
                if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
                else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
                else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
                ++count;
                c = lex->ctx[++lex->str_count];
            }
            lex->position += count;
            token tok = { { .val.int64 = val } , lex->line, lex->position - count - 2, TokenTypeLiteralInt };
            tok.data.type = PrimitiveDataTypeInt64;
            END_PROFILING(__func__);
            return tok;
        }
        case 'b': {
            c = lexer_get_consumed(lex);
            while (c == '0' || c == '1') {
                val = val * 2 + (c - '0');
                ++count;
                c = lex->ctx[++lex->str_count];
            }
            lex->position += count;
            token tok = { { .val.int64 = val } , lex->line, lex->position - count - 2, TokenTypeLiteralInt };
            tok.data.type = PrimitiveDataTypeInt64;
            END_PROFILING(__func__);
            return tok;
        }
        default:
            // exit(1);
            // TODO: error undefine notation
        break;
        }
    }
    while (c >= '0' && c <= '9') {
        val = val * 10 + (c - '0');
        ++count;
        c = lex->ctx[++lex->str_count];
        if (percision_count > 0) {
            percision_count *= 10;
            continue;
        }
        if (c == '.') {
            if (percision_count == 0) {
                ++count;
                c = lex->ctx[++lex->str_count];
                percision_count = 1;
            }
            else {
                // TODO: error having multiple . 
                // exit(1);
            }
        }
    }
    if (count <= 0) {
        // WARN: error undefine behaviours
        // exit(1);
    }
    lex->position += count;
    if (percision_count == 0) {
        token tok = { { .val.int64 = val } , lex->line, lex->position - count, TokenTypeLiteralInt };
        tok.data.type = PrimitiveDataTypeInt64;
        END_PROFILING(__func__);
        return tok;
    }
    token tok = { { .val.float64 = (f64)val / percision_count } , lex->line, lex->position - count - 1, TokenTypeLiteralFloat };
    tok.data.type = PrimitiveDataTypeFloat64;
    END_PROFILING(__func__);
    return tok;
}

static token generate_text_token(lexer* lex) {
    START_PROFILING();
    i32 count = 0;
    char c = lex->ctx[lex->str_count];
    const char* name_begin = lex->ctx + lex->str_count;
    do {
        ++count;
        c = lex->ctx[++lex->str_count];
    } while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');

    TokenType tok_type = TokenTypeIdentifier;
    for (i32 type = TokenTypeKeywordVar; type <= TokenTypeKeywordFor; ++type) {
        if ((i32)strlen(TokenTypeString[type - 256]) == count && strncmp(TokenTypeString[type - 256], name_begin, count) == 0) {
            tok_type = type;
            break;
        }
    }

    lex->position += count;
    if (tok_type != TokenTypeIdentifier) {
        END_PROFILING(__func__);
        return (token) {
            { .val.string = NULL }, lex->line, lex->position - count, tok_type
        };
    }

    END_PROFILING(__func__);
    return (token) {
        { .val.string = make_stringn(name_begin, count) }, lex->line, lex->position - count, TokenTypeIdentifier
    };
}

#define MATCH_ONE_AFTER(c1, c2, tok_type)\
    START_PROFILING();\
    token tok = { { .val.string = NULL }, lex->line, lex->position, tok_type};\
    if (lex->ctx[lex->str_count + 1] == c2) {\
        vector_push(result, tok);\
        lexer_consume_n(lex, 2);\
        END_PROFILING("gen " #c2);\
        break;\
    }\
    tok.type = c1;\
    vector_push(result, tok);\
    lexer_consume(lex);\
    END_PROFILING("gen " #tok_type);\
    break;

vector(token) generate_tokens(lexer* lex) {
    vector(token) result = make_vector(token);
    START_PROFILING();
    while (1) {
        char c = lex->ctx[lex->str_count];
        switch (c) {
        case '(': case ')':
        case '[': case ']':
        case '{': case '}':
        case ';': case ':': case '\'': case '"': case '\\': case ',':
        case '\t': { 
            START_PROFILING();
            token tok = { { .val.string = NULL }, lex->line, lex->position, c};
            vector_push(result, tok);
            lexer_consume(lex);
            END_PROFILING("gen token separator");
            break;
        }
        case '\n': { 
            START_PROFILING();
            token tok = { { .val.string = NULL }, lex->line++, lex->position, c};
            vector_push(result, tok);
            lex->position = 1;
            lexer_consume(lex);
            END_PROFILING("gen token newline");
            break;
        }
        case ' ': case '\r': { lexer_consume(lex); break; }
        case '>': { MATCH_ONE_AFTER(c, '=', TokenTypeOperatorGreaterThan); }
        case '<': { MATCH_ONE_AFTER(c, '=', TokenTypeOperatorLessThan); }
        case '=': { MATCH_ONE_AFTER(c, '=', TokenTypeOperatorEqual); }
        case '!': { MATCH_ONE_AFTER(c, '=', TokenTypeOperatorNotEqual); }
        case '+': { MATCH_ONE_AFTER(c, '=', TokenTypeAssignmentPlus); } 
        case '-': { MATCH_ONE_AFTER(c, '=', TokenTypeAssignmentMinus); } 
        case '*': { MATCH_ONE_AFTER(c, '=', TokenTypeAssignmentMultiply); }
        case '/': { MATCH_ONE_AFTER(c, '=', TokenTypeAssignmentDivide); } 
        case '%': { MATCH_ONE_AFTER(c, '=', TokenTypeAssignmentModulus); }
        case 0: { 
            token tok = { { .val.string = NULL }, lex->line, lex->position, TokenTypeEOF};
            vector_push(result, tok);
            lexer_consume(lex);
            END_PROFILING(__func__);
            return result;
        }
        case '.': {
            if (is_0_9(lex->ctx[lex->str_count + 1])) {
                lexer_consume(lex);
                token tok = generate_float_after_dot(lex);
                vector_push(result, tok);
                continue;
            }
            token tok = { { .val.string = NULL }, lex->line, lex->position, c};
            vector_push(result, tok);
            lexer_consume(lex);
            break; 
        }
        default: {
            if (c >= '0' && c <= '9') {
                token tok = generate_number_literal_token(lex);
                vector_push(result, tok);
            }
            else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
                token tok = generate_text_token(lex);
                vector_push(result, tok);
            }
            else {
                LOG_ERROR("unkwon ascci %d\n", (u8)c);
                ASSERT_MSG(0, "unkown symbol");
            }
            break;
        }
        }
    }
    END_PROFILING(__func__);
    return result;
}

