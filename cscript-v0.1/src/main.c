#include <stdio.h>
#include <string.h>
#include "container/string.h"
#include "container/memallocate.h"

vector(char) load_file(const char* file_name, const char* mode) {
    FILE* file = fopen(file_name, mode);
    if (!file) {
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    fseek(file, 0, SEEK_SET);

    vector(char) result = make_vector();
    vector_resize(result, size + 1);
    fread(result, 1, size, file);
    result[size] = EOF;
    fclose(file);
    return result;
}

typedef enum {
    TokenTypeIdentifier = 256,
    TokenTypeComment,

    TokenTypeKeywordVar,
    TokenTypeKeywordFun,
    TokenTypeKeywordIf,
    TokenTypeKeywordElif,
    TokenTypeKeywordElse,
    TokenTypeKeywordWhile,
    TokenTypeKeywordFor,

    TokenTypeLiteralString,
    TokenTypeLiteralInt32,
    TokenTypeLiteralFloat32,

    TokenTypeEOF,
} TokenType;

const char* TokenTypeString[] = {
    [TokenTypeIdentifier] = "",
    [TokenTypeComment] = "",

    [TokenTypeKeywordVar] = "var",
    [TokenTypeKeywordFun] = "fun",
    [TokenTypeKeywordIf] = "if",
    [TokenTypeKeywordElif] = "elif",
    [TokenTypeKeywordElse] = "else",
    [TokenTypeKeywordWhile] = "while",
    [TokenTypeKeywordFor] = "for",

    [TokenTypeLiteralString] = "",
    [TokenTypeLiteralInt32] = "",
    [TokenTypeLiteralFloat32] = "",

    [TokenTypeEOF] = "EOF",
};

typedef struct {
    union {
        const cstring name;
        i32 int32;
        f32 float32;
    } val;
    i32 line, count;
    TokenType type;
} token;

typedef struct {
    const char* ctx;
    i32 ctx_len;
    i32 line, position, str_count;
} lexer;

static void lexer_load_file_text(lexer* lex, const char* file_name) {
    lex->ctx = load_file(file_name, "r");
    lex->ctx_len = strlen(lex->ctx);
}

static void lexer_consume(lexer* lex) {
    ++lex->str_count;
    ++lex->position;
}

static char lexer_get_consumed(lexer* lex) {
    lexer_consume(lex);
    return lex->ctx[lex->str_count];
}

static i32 is_0_9(char ch) {
    return ch >= '0' && ch <= '9';
}

static token generate_number_literal_token(lexer* lex) {
    i32 val = 0, count = 0, percision_count = 0;
    char c = lex->ctx[lex->str_count];
    if (c == '0' && !is_0_9(lex->ctx[lex->str_count + 1])) {
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
            return (token) {
                .val.int32 = val, lex->line, lex->position - count - 2, TokenTypeLiteralInt32
            };
            break;
        }
        case 'b': {
            c = lexer_get_consumed(lex);
            while (c == '0' || c == '1') {
                val = val * 2 + (c - '0');
                ++count;
                c = lex->ctx[++lex->str_count];
            }
            lex->position += count;
            return (token) {
                .val.int32 = val, lex->line, lex->position - count - 2, TokenTypeLiteralInt32
            };
            break;
        }
        default:
            exit(1);
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
                exit(1);
            }
        }
    }
    if (count <= 0) {
        // WARN: error undefine behaviours
        exit(1);
    }
    lex->position += count;
    if (percision_count == 0) {
        return (token) {
            .val.int32 = val, lex->line, lex->position - count, TokenTypeLiteralInt32
        };
    }
    return (token) {
        .val.float32 = (f32)val / percision_count, lex->line, lex->position - count, TokenTypeLiteralFloat32
    };
}

static token generate_text_token(lexer* lex) {
    i32 count = 0;
    char c = lex->ctx[lex->str_count];
    const char* name_begin = lex->ctx + lex->str_count;
    do {
        ++count;
        c = lex->ctx[++lex->str_count];
    } while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');

    TokenType tok_type = TokenTypeIdentifier;
    for (i32 type = TokenTypeKeywordVar; type <= TokenTypeKeywordFor; ++type) {
        if ((i32)strlen(TokenTypeString[type]) == count && strncmp(TokenTypeString[type], name_begin, count) == 0) {
            tok_type = type;
            break;
        }
    }

    lex->position += count;
    if (tok_type != TokenTypeIdentifier) {
        return (token) {
            .val.name = NULL, lex->line, lex->position - count, tok_type
        };
    }

    return (token) {
        .val.name = make_stringn(name_begin, count), lex->line, lex->position - count, TokenTypeIdentifier
    };
}

vector(token) generate_tokens(lexer* lex) {
    vector(token) result = make_vector();
    while (1) {
        char c = lex->ctx[lex->str_count];
        switch (c) {
        case '(': { vector_push(result, .val.name = NULL, lex->line, lex->position, '('); lexer_consume(lex); break; }
        case ')': { vector_push(result, .val.name = NULL, lex->line, lex->position, ')'); lexer_consume(lex); break; }
        case '[': { vector_push(result, .val.name = NULL, lex->line, lex->position, '['); lexer_consume(lex); break; }
        case ']': { vector_push(result, .val.name = NULL, lex->line, lex->position, ']'); lexer_consume(lex); break; }
        case '{': { vector_push(result, .val.name = NULL, lex->line, lex->position, '{'); lexer_consume(lex); break; }
        case '}': { vector_push(result, .val.name = NULL, lex->line, lex->position, '}'); lexer_consume(lex); break; }
        case ';': { vector_push(result, .val.name = NULL, lex->line, lex->position, ';'); lexer_consume(lex); break; }
        case ':': { vector_push(result, .val.name = NULL, lex->line, lex->position, ':'); lexer_consume(lex); break; }
        case '\'': { vector_push(result, .val.name = NULL, lex->line, lex->position, '\''); lexer_consume(lex); break; }
        case '"': { vector_push(result, .val.name = NULL, lex->line, lex->position, '"'); lexer_consume(lex); break; }
        case '\\': { vector_push(result, .val.name = NULL, lex->line, lex->position, '\\'); lexer_consume(lex); break; }
        case ',': { vector_push(result, .val.name = NULL, lex->line, lex->position, ','); lexer_consume(lex); break; }
        case '.': { vector_push(result, .val.name = NULL, lex->line, lex->position, '.'); lexer_consume(lex); break; }
        case '\n': { vector_push(result, .val.name = NULL, lex->line++, lex->position, '\n'); lexer_consume(lex); lex->position = 1; break; }
        case '\t': { vector_push(result, .val.name = NULL, lex->line++, lex->position, '\t'); lexer_consume(lex); lex->position = 1; break; }
        case '+': { vector_push(result, .val.name = NULL, lex->line, lex->position, '+'); lexer_consume(lex); break; }
        case '*': { vector_push(result, .val.name = NULL, lex->line, lex->position, '*'); lexer_consume(lex); break; }
        case '-': { vector_push(result, .val.name = NULL, lex->line, lex->position, '-'); lexer_consume(lex); break; }
        case '/': { vector_push(result, .val.name = NULL, lex->line, lex->position, '/'); lexer_consume(lex); break; }
        case '=': { vector_push(result, .val.name = NULL, lex->line, lex->position, '='); lexer_consume(lex); break; }
        case '>': { vector_push(result, .val.name = NULL, lex->line, lex->position, '>'); lexer_consume(lex); break; }
        case '<': { vector_push(result, .val.name = NULL, lex->line, lex->position, '<'); lexer_consume(lex); break; }
        case ' ': { lexer_consume(lex); break; }
        case EOF: { vector_push(result, .val.name = NULL, lex->line, lex->position, TokenTypeEOF); return result; }
        default: {
            if (c >= '0' && c <= '9') {
                token tok = generate_number_literal_token(lex);
                vector_pushe(result, tok);
            }
            else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
                token tok = generate_text_token(lex);
                vector_pushe(result, tok);
            }
            else {
                printf("unkown symbol");
                exit(1);
            }
            if (c == 0)
                return result;
            break;
        }

        }
    }
    return result;
}

int main(void) {
    lexer lex = {NULL, -1, 1, 1, 0};
    lexer_load_file_text(&lex, "test.cscript");
    vector(token) tokens = generate_tokens(&lex);
    for_vector(tokens, i, 0) {
        char buf[100];
        sprintf(buf, "%d:%d:%d ", tokens[i].line, tokens[i].count, tokens[i].type);
        printf("%s %*c", buf, 14 - (i32)strlen(buf), ' ');
        if (tokens[i].type == TokenTypeLiteralInt32) {
            printf("i32: %d\n", tokens[i].val.int32);
        }
        else if (tokens[i].type == TokenTypeLiteralFloat32) {
            printf("f32: %g\n", tokens[i].val.float32);
        }
        else if (tokens[i].type == TokenTypeIdentifier) {
            printf("id:  %s len: %d\n", tokens[i].val.name, (i32)strlen(tokens[i].val.name));
            free_string(tokens[i].val.name);
        }
        else {
            if (tokens[i].type < 256)
                printf("sym: '%c' asc: %d\n", tokens[i].type == '\n' ? ' ' : tokens[i].type, tokens[i].type);
            else
                printf("key: %s\n", TokenTypeString[tokens[i].type]);
        }
    }
    free_vector(tokens);
    free_vector(lex.ctx);
    printf("leak count = %d\n", check_memory_leak());
    return 0;
}
