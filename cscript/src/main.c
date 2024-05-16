#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vector.h"
#include "memallocate.h"

#define MAX_LINE_BUFFER_SIZE (1 << 10)
#define MAX_WORD_BUFFER_SIZE (1 << 8)

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
    const char* name;
    i32 name_len;
    i32 name_location;
    i32 type;
} token;

typedef struct {
    const char** set_name;
    u64 set_size;
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

i32 match_token(token_set* token_set, const char* str) {
    for (u64 i = 0; i < token_set->set_size; ++i) {
        char* cp = strchr(str, token_set->set_name[i][0]);
        if (cp != NULL) {
            u64 len = strlen(token_set->set_name[i]);
            if (strncmp(token_set->set_name[i], cp, len) == 0) {
                return cp - str + 1;
            }
        }
    }
    return -1;
}

i32 compare_token_set(token_set* token_set, const char* str) {
    for (u64 i = 0; i < token_set->set_size; ++i) {
        u64 len = strlen(token_set->set_name[i]);
        if (strncmp(token_set->set_name[i], str, len) == 0) {
            return 1;
        }
    }
    return 0;
}

token compare_with_token_sets(lexer* lexer, const char* str) {
    for (u64 i = 0; i < TokenParserCount; ++i) {
        for (u64 j = 0; j < lexer->token_sets[i].set_size; ++j) {
            u64 len = strlen(lexer->token_sets[i].set_name[j]);
            if (strncmp(lexer->token_sets[i].set_name[j], str, len) == 0) {
                return (token){
                    .name = str,
                    .name_len = len,
                    .name_location = j,
                    .type = lexer->token_sets[i].token,
                };
            }
        }
    }
    return (token){ .name = str, .name_len = -1, .type = -1, .name_location = -1 };
}

i32 is_alphabet(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
i32 is_number(char c) { return c >= '0' && c <= '9'; }

i32 get_word_stride(lexer* lexer, const char* str) {
    if (str[0] == 0)
        return -1;

    for (u64 j = 0; j < lexer->token_sets[TokenSeparator].set_size; ++j) {
        if (str[0] == lexer->token_sets[TokenSeparator].set_name[j][0]) {
            return 2;
        }
    }

    if (is_alphabet(str[0])) {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (!is_alphabet(str[i]) && !is_number(str[i])) {
                return i + 1;
            }
        }
    }
    else if (is_number(str[0])) {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (compare_token_set(&lexer->token_sets[TokenSeparator], str + 1)) {
                return i + 2;
            }
            // forget what this code is for
            // for (u64 j = 0; j < lexer->token_sets[TokenOperator].set_size; ++j) {
            //     if (str[i + 1] == lexer->token_sets[TokenOperator].set_name[j][0]) {
            //         return i + 2;
            //     }
            // }

            if (str[i] == ' ' || str[i] == '\n') {
                return i + 1;
            }
        }
    }
    else {
        for (u64 i = 0; str[i] != 0; ++i) {
            if (compare_token_set(&lexer->token_sets[TokenSeparator], str + 1)) {
                return i + 2;
            }
            if (is_alphabet(str[i]) || is_number(str[i]) || str[i] == ' ') {
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
        nword_len = get_word_stride(lexer, str + str_begin_offset) - 1;
        if (nword_len < 0) {
            return tokens;
        }

        if (*(str + str_begin_offset) == ' ') {
            str_begin_offset++;
            continue;
        }

        token result = compare_with_token_sets(lexer, str + str_begin_offset);
        str_begin_offset += nword_len;
        if (result.type >= 0) {
            vector_pushe(tokens, result);
            continue;
        }
        else {
            if (is_number(result.name[0])) {
                result.type = TokenLiteral;
            }
            else {
                result.type = TokenIdentifier;
            }
            result.name_len = nword_len;
            vector_pushe(tokens, result);
        }
        // vector_push(tokens, str + str_begin_offset, nword_len, -1, -1);
    }

    return tokens;
}

typedef struct {
    char* buffer;
    u64 buffer_size, line_count;
} source_file;

i32 is_space_strn(const char* str, u64 size) {
    for (u64 i = 0; i < size; ++i) {
        if (str[i] != ' ') {
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
        u64 line_len = strlen(line_buffer);

        i32 sc_pos = match_token(single_line_comment, line_buffer);
        if (sc_pos >= 0) {
            if (sc_pos == 1 || is_space_strn(line_buffer, sc_pos - 2))
                continue;
            line_len = sc_pos - 1;
            line_buffer[line_len++] = '\n';
        }

        // printf("%llu\n", line_len);
        // line_len++;
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

void free_source(source_file* source) {
    assert(source->buffer);
    free(source->buffer);
    memset(source, 0, sizeof(source_file));
}

const char* Keyword[] = {
    "if", "while", "return", "const",
    "var", "int", "float", "true", "false",
    "then", "end", "def", "->", "**"
};

// must be in one character
const char* Separator[] = {
    "[", "]", "(", ")", ":", ",", "\n", "\"",
};

const char* Operator[] = {
    "=", "+", "-", "*", "/", "==", "+=", "-=", "*=", "/=", "|", "&", "~", "<<", ">>", 
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
    printf("(%s, '", type_name);
    for (i32 n = 0; n < token->name_len; ++n) {
        if (token->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(token->name[n]);
    }
    printf("', %d)\n", token->name_len);
    // printf(", %d, %s", token->name_len, type_name);
}

void print_token(token* token) {
    switch (token->type) {
    case TokenKeyword: _print_token(token, "keyword"); break;
    case TokenSeparator: _print_token(token, "separator"); break;
    case TokenOperator: _print_token(token, "operator"); break;
    case TokenLiteral: _print_token(token, "literal"); break;
    case TokenIdentifier: _print_token(token, "identifier"); break;
    default: _print_token(token, "unkown"); break;
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

    // TODO: to pack source file
    source_file source;
    i32 success = load_source(&source, argv[1], &(token_set){
                .set_name = SingleLineComment,
                .set_size = sizeof(SingleLineComment) / sizeof(void*)
            });

    if (!success) {
        printf("failed load source\n");
        exit(1);
    }

    printf("size = %llu, line count = %llu\n", source.buffer_size, source.line_count);
    printf("----- source begin ------\n\
            %s\
            ----- source end -----\n\n", source.buffer);

    u64 offset = 0;
    u64 first_n = 0;
    for (u64 i = 0; i < source.line_count; ++i) {
        first_n = strchr(source.buffer + offset, '\n') - source.buffer - offset + 1;

        printf("<line:%llu> ", i);
        for (u64 c = 0; c < first_n; ++c) {
            putchar((source.buffer + offset)[c]);
        }

        vector(token) tokens = lexer_tokenize_str(&lexer, source.buffer + offset, first_n);
        for (u64 j = 0; j < vector_size(tokens); ++j) {
            printf("[%llu:%llu]", i, tokens[j].name - source.buffer - offset + 1);
            print_token(&tokens[j]);
        }
        //putchar('\n');
        free_vector(tokens);
        offset += first_n;
    }

    free_source(&source);
    CHECK_MEMORY_LEAK();
}

