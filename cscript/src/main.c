#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vector.h"
#include "memallocate.h"

#define MAX_LINE_BUFFER_SIZE (1 << 10)
#define MAX_WORD_BUFFER_SIZE (1 << 8)
#define SPACE (' ')
#define TERMINATION ('\0')
#define NEXTLINE ('\n')

#define TOKEN(x, name) { x, #name, sizeof(#name) }

const char* type[] = {
    "int",
    "float",
};

const char* keyword[] = {
    "const",
    "for",
    "if",
    "elif",
    "else",
};

const char* operator[] = {
    "=",
    "+",
    "-",
    "*",
    "/",
    "+=",
    "-=",
    "*=",
    "/=",
};

typedef struct {
    const char* name;
    i32 name_len;
    i32 name_location;
    i32 type;
} token;

void parser_word(const char word[MAX_WORD_BUFFER_SIZE], i32 size) {
    (void)word, (void)size;
}

void parser_line(char* buffer, i32 size) {
    char word_buffer[MAX_WORD_BUFFER_SIZE];
    memset(word_buffer, 0, MAX_WORD_BUFFER_SIZE);

    i32 count = 0;

    /*  value
        type identifier operator rvalue -> init value ex: Int a = 1\n Int b = a
        type identifier                 -> declaration ex: Int a (Int a = 0)
        init: error check -> push to stack -> save data info
        release: retrieve data info -> pop from stack
        identifier operator rvalue      -> operation ex: Int a\n Int b\n a = b
        retrieve data info -> operate
    */

    for (i32 i = 0; i < size - 1; ++i) { 
        if (buffer[i] != SPACE && buffer[i] != NEXTLINE) {
            word_buffer[count++] = buffer[i];
            continue;
        }
        if (buffer[i + 1] == SPACE || count == 0) {
            continue;
        }
        word_buffer[count++] = 0;
        // parser_word(word_buffer, count);
        printf(" '%s' %d\n", word_buffer, count);
        memset(word_buffer, 0, count);
        count = 0;
    }

    putchar(NEXTLINE);
}

typedef struct {
    char* buffer;
    u64 buffer_size, line_count;
} source_file;

i32 load_source(source_file* source, const char* path) {
        FILE* file = fopen(path, "r");
        if (file == NULL) {
            printf("cannot fount %s\n", path);
            return 0;
        }

        fseek(file, 0, SEEK_END);
        source->buffer_size = ftell(file);
        rewind(file);

        source->buffer = malloc(source->buffer_size);

        u64 line_count, count;
        line_count = count = 0;
        char line_buffer[MAX_LINE_BUFFER_SIZE];
        while (fgets(line_buffer, MAX_LINE_BUFFER_SIZE, file)) {
            u64 line_len = strlen(line_buffer);
            memcpy(source->buffer + count, line_buffer, line_len);
            count += line_len;
            ++line_count;
        }

        source->line_count = line_count;

        fclose(file);

        return 1;
}

void free_source(source_file* source) {
    assert(source->buffer);
    free(source->buffer);
    memset(source, 0, sizeof(source_file));
}

// lexer implementation
typedef enum {
    TokenKeyword = 0,
    TokenSeparator,
    TokenOperator,
    TokenParserCount,

    TokenComment,
    TokenLIteral,
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

token compare_with_token_set_name(token_set* token_set, const char* str, u64 str_len) {
    for (u64 i = 0; i < token_set->set_size; ++i) {
        if (strncmp(token_set->set_name[i], str, str_len) == 0) {
            return (token){
                .name = str,
                .name_len = str_len,
                .name_location = i,
                .type = token_set->token,
            };
        }
    }
    return (token){ .name = NULL };
}

i32 get_word_len(token_set* separator_set, const char* str) {
    if (str[0] == 0)
        return -1;

    u64 result = -1;
    char* cp;

    for (u64 i = 0; i < separator_set->set_size; ++i) {
        cp = strchr(str, separator_set->set_name[i][0]);
        if (cp != NULL) {
            u64 plen = cp - str + 1;
            if (plen < result)
                result = plen;
        }
    }

    cp = strchr(str, ' ');
    if (cp != NULL) {
        u64 plen = cp - str + 1;
        if (plen < result)
            result = plen;
    }

    return result;
}

vector(token) lexer_tokenize_str(lexer* lexer, const char* str) {
    assert(lexer != NULL);

    i32 str_begin_offset = 0;
    i32 nword_len = 0;

    vector(token) tokens = make_vector();

    for (;;) {
        nword_len = get_word_len(&lexer->token_sets[TokenSeparator], str + str_begin_offset) - 1;

        if (*(str + str_begin_offset) == ' ') {
            str_begin_offset++;
            continue;
        }

        if (nword_len == 0) {
            nword_len++;
        }

        if (nword_len < 0) {
            return tokens;
        }

        // make it more readable and cool!
        int have_one = 0;
        for (u64 i = 0; i < TokenParserCount; ++i) {
            token result = compare_with_token_set_name(&lexer->token_sets[i], str + str_begin_offset, nword_len);
            if (result.name != NULL) {
                vector_push(tokens, result.name, result.name_len, result.name_location, result.type);
                have_one = 1;
                break;
            }
        }

        if (have_one == 0) {
            vector_push(tokens, str + str_begin_offset, nword_len, -1, -1);
        }

        str_begin_offset += nword_len;
    }

    return tokens;
}

const char* Keyword[] = {
    "if", "while", "return"
};

const char* Separator[] = {
    "[", "]", "(", ")", ":", "\n"
};

// must be one character
const char* Operator[] = {
    "=", "+", "-", "*", "/", "==", "+=", "-=", "*=", "/=",
};

const char* Comment[] = {
    "/*", "*/"
};

#define LEXER_ADD_TOKEN(lexer, str_set, type)\
    lexer_add_token((lexer), (token_set){\
                .set_name = str_set,\
                .set_size = sizeof(str_set) / sizeof(void*),\
                .token = type\
            }, type);

void _print_token(token* token, const char* type_name) {
    for (i32 n = 0; n < token->name_len; ++n) {
        putchar(token->name[n]);
    }
    printf(", name len = %d, token type = %d, type name = %s\n", token->name_len, token->type, type_name);
}

void print_token(token token) {
    switch (token.type) {
    case TokenKeyword: _print_token(&token, "keyword"); break;
    case TokenSeparator: _print_token(&token, "separator"); break;
    case TokenOperator: _print_token(&token, "operator"); break;
    case TokenComment: _print_token(&token, "comment"); break;
    default: _print_token(&token, "?"); break;
    }
}

i32 main(i32 argc, char** argv) {
    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, Comment, TokenComment);

    const char test_line[] = " /* if (a == b)a\n";
    const char* str_begin = &test_line[0];

    // ------ prepack line ------
    
    vector(token) tokens = lexer_tokenize_str(&lexer, str_begin);

    for (u64 i = 0; i < vector_size(tokens); ++i) {
        print_token(tokens[i]);
    }

    free_vector(tokens);

    CHECK_MEMORY_LEAK();

    return 0;
    if (argc == 1) {
        printf("error: no file input\n");
        exit(1);
    }

    source_file source;
    i32 success = load_source(&source, argv[1]);
    if (!success) {
        printf("failed load source\n");
        exit(1);
    }

    printf("\nsize = %llu\nline count = %llu\n", source.buffer_size, source.line_count);

    u64 count = 0;
    i32 line_count = 0;
    char line_buffer[MAX_LINE_BUFFER_SIZE];

    for (u64 i = 0; i < source.line_count; i++) {
        char c;
        do {
            c = line_buffer[line_count++] = source.buffer[count++];
        } while(c != NEXTLINE);
        line_buffer[line_count++] = NEXTLINE;
        parser_line(line_buffer, line_count);
        line_count = 0;
    }
    free_source(&source);
}
