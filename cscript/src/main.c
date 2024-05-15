#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vector.h"
#include "memallocate.h"

#define MAX_LINE_BUFFER_SIZE (1 << 10)
#define MAX_WORD_BUFFER_SIZE (1 << 8)

typedef struct {
    const char* name;
    i32 name_len;
    i32 name_location;
    i32 type;
} token;

// lexer implementation
typedef enum {
    TokenKeyword = 0,
    TokenSeparator,
    TokenOperator,
    TokenParserCount,

    TokenLiteral,
    TokenIdentifier,
    TokenCount,
} Token;

typedef struct {
    const char** set_name;
    u32 set_size;
    Token token;
} token_set;

typedef struct {
    token_set token_sets[TokenCount];
} lexer;

void lexer_add_token(lexer* lexer, token_set set, Token token) {
    assert(lexer != NULL);
    assert(token >=0 && token < TokenCount);
    lexer->token_sets[token] = set;
}

token compare_with_token_set(lexer* lexer, const char* str, u64 str_len) {
    for (u64 i = 0; i < TokenParserCount; ++i) {
        for (u64 j = 0; j < lexer->token_sets[i].set_size; ++j) {
            if (strncmp(lexer->token_sets[i].set_name[j], str, str_len) == 0) {
                return (token){
                    .name = str,
                    .name_len = str_len,
                    .name_location = j,
                    .type = lexer->token_sets[i].token,
                };
            }
        }
    }
    return (token){ .name = NULL, .name_len = 0, .type = -1 };
}

i32 is_literal(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
i32 is_number(char c) { return c >= '0' && c <= '9'; }

i32 get_next_word_len(const char* str) {
    if (str[0] == 0)
        return -1;

    if (is_literal(str[0])) {
        for (u64 i = 0; str[i] != 0; i++) {
            if (!is_literal(str[i]) && !is_number(str[i])) {
                return i + 1;
            }
        }
    }
    else if (is_number(str[0])) {
        for (u64 i = 0; str[i] != 0; i++) {
            if (!is_number(str[i])) {
                return i + 1;
            }
        }
    }
    else {
        for (u64 i = 0; str[i] != 0; i++) {
            if (is_literal(str[i]) || is_number(str[i]) || str[i] == ' ') {
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
        str_begin_offset += nword_len;
        nword_len = get_next_word_len(str + str_begin_offset) - 1;
        if (nword_len < 0) {
            return tokens;
        }

        if (*(str + str_begin_offset) == ' ') {
            str_begin_offset++;
            continue;
        }

        token result = compare_with_token_set(lexer, str + str_begin_offset, nword_len);
        if (result.name != NULL) {
            vector_push(tokens, result.name, result.name_len, result.name_location, result.type);
            continue;
        }
        vector_push(tokens, str + str_begin_offset, nword_len, -1, -1);
    }

    return tokens;
}

typedef struct {
    char* buffer;
    u64 buffer_size, line_count;
} source_file;

i32 match_token(token_set* single_line_comment, const char* str) {
    for (u64 i = 0; i < single_line_comment->set_size; ++i) {
        char* cp = strchr(str, single_line_comment->set_name[i][0]);
        if (cp != NULL) {
            u64 len = strlen(single_line_comment->set_name[i]);
            if (strncmp(single_line_comment->set_name[i], cp, len) == 0) {
                return cp - str + 1;
            }
        }
    }
    return -1;
}

i32 is_empty_strn(const char* str, u64 size) {
    for (u64 i = 0; i < size; ++i) {
        if (!is_literal(str[i])) {
            return 0;
        }
    }
    return 1;
}

i32 load_source(source_file* source, const char* path, token_set* single_line_comment) {
        FILE* file = fopen(path, "r");
        if (file == NULL) {
            printf("cannot fount %s\n", path);
            return 0;
        }

        fseek(file, 0, SEEK_END);
        source->buffer_size = ftell(file);
        rewind(file);

        source->buffer = malloc(source->buffer_size + 1);

        u64 line_count = 0, count = 0;
        char line_buffer[MAX_LINE_BUFFER_SIZE];

        while (fgets(line_buffer, MAX_LINE_BUFFER_SIZE, file)) {
            i32 sc_pos = match_token(single_line_comment, line_buffer);
            u64 line_len = strlen(line_buffer);
            if (sc_pos >= 0) {
                line_len = sc_pos - 1;
                // TODO: \n\r is only for windows and unix but mac is using \n! 
                // try to make it cross platform!
                line_buffer[line_len++] = '\n';
                line_buffer[line_len++] = '\r';
            }

            if (line_len == 2 || is_empty_strn(line_buffer, line_len)) {
                continue;
            }

            memcpy(source->buffer + count, line_buffer, line_len);
            count += line_len;
            ++line_count;
        }

        source->buffer_size = count;
        source->line_count = line_count;

        source->buffer[source->buffer_size] = 0;

        fclose(file);

        return 1;
}

// void pack_source(source_file* source, token_set* comment_set) {
// }

void free_source(source_file* source) {
    assert(source->buffer);
    free(source->buffer);
    memset(source, 0, sizeof(source_file));
}

const char* Keyword[] = {
    "if", "while", "return", "const",
    "int", "float"
};

// must be in one character
const char* Separator[] = {
    "[", "]", "(", ")", ":", "\n"
};

const char* Operator[] = {
    "=", "+", "-", "*", "/", "==", "+=", "-=", "*=", "/=", "|", "&", "~", "<<", ">>"
};

const char* SingleLineComment[] = {
    "//"
};

// must be a string pair
const char* MultiLineComment[] = {
    "/*", "*/"
};


#define LEXER_ADD_TOKEN(lexer, str_set, type)\
    lexer_add_token((lexer), (token_set){\
                .set_name = str_set,\
                .set_size = sizeof(str_set) / sizeof(void*),\
                .token = type\
            }, type);

void _print_token(token* token, const char* type_name) {
    printf("name = ");
    for (i32 n = 0; n < token->name_len; ++n) {
        putchar(token->name[n]);
    }
    printf(", name len = %d, token type = %d, type name = %s\n", token->name_len, token->type, type_name);
}

void print_token(token* token) {
    switch (token->type) {
    case TokenKeyword: _print_token(token, "keyword"); break;
    case TokenSeparator: _print_token(token, "separator"); break;
    case TokenOperator: _print_token(token, "operator"); break;
    default: _print_token(token, "?"); break;
    }
}

i32 main(i32 argc, char** argv) {
    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);

    if (argc == 1) {
        printf("error: no file input\n");
        exit(1);
    }

    source_file source;
    i32 success = load_source(&source, argv[1], &(token_set){
                .set_name = SingleLineComment,
                .set_size = sizeof(SingleLineComment) / sizeof(void*)
            });
    if (!success) {
        printf("failed load source\n");
        exit(1);
    }


    // TODO: pack source file

    printf("size = %llu, line count = %llu\n", source.buffer_size, source.line_count);
    printf("---- \n%s----\n\n", source.buffer);

    u64 first_n = strchr(source.buffer, '\n') - source.buffer + 1;
    printf("line: ");
    for (u64 i = 0; i < first_n; ++i) {
        putchar(source.buffer[i]);
    }

    vector(token) tokens = lexer_tokenize_str(&lexer, source.buffer, first_n);
    for (u64 i = 0; i < vector_size(tokens); ++i) {
        print_token(&tokens[i]);
    }
    free_vector(tokens);

    free_source(&source);
    CHECK_MEMORY_LEAK();
}

