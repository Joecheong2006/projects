#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "hashmap.h"
#include "memallocate.h"
#include "lexer.h"

#define MAX_LINE_BUFFER_SIZE (1 << 10)
#define MAX_WORD_BUFFER_SIZE (1 << 8)

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

i32 load_source(source_file* source, const char* path, token_set* single_line_comment, token_set* multi_line_comment) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("cannot fount %s\n", path);
        return 0;
    }
    // const char* hello = "hi";
    (void)single_line_comment;
    (void)multi_line_comment;

    fseek(file, 0, SEEK_END);
    source->buffer_size = ftell(file);
    rewind(file);

    source->buffer = MALLOC(source->buffer_size + 1);

    u64 line_count = 0, count = 0;
    char line_buffer[MAX_LINE_BUFFER_SIZE];

    while (fgets(line_buffer, MAX_LINE_BUFFER_SIZE, file)) {
        u64 line_len = strlen(line_buffer);

        // i32 sc_pos = match_token(single_line_comment, line_buffer);
        // if (sc_pos >= 0) {
        //     if (sc_pos == 1 || is_space_strn(line_buffer, sc_pos - 2))
        //         continue;
        //     line_len = sc_pos - 1;
        //     line_buffer[line_len++] = '\n';
        // }

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
    FREE(source->buffer);
    memset(source, 0, sizeof(source_file));
}

enum {
    If,
    While,
    Return,
    Const,
    Int,
    Float,
    String,
    Char,
    True,
    Flase,
    None,
    Then,
    End,
    Function,
    ReturnCarry,
    Power
};

const char* Keyword[] = {
    [If] = "if",
    [While] = "while",
    [Return] = "return",
    [Const] = "const",
    [Int] = "int", 
    [Float] = "float", 
    [Char] = "char",
    [String] = "string",
    [True] = "None", 
    [Flase] = "true", 
    [None] = "false",
    [Then] = "then", 
    [End] = "end", 
    [Function] = "function", 
    [ReturnCarry] = "->", 
    [Power] = "**"
};

enum {
    OpenSquareBracket,
    CloseSquareBracket,
    OpenRoundBracket,
    CloseRoundBracket,
    Colon,
    Comma,
    SingleQuote,
    DoubleQuote,
    NewLine,
    Space
};

// must be in one character
const char* Separator[] = {
    [OpenSquareBracket] = "[",
    [CloseSquareBracket] = "]",
    [OpenRoundBracket] = "(",
    [CloseRoundBracket] = ")",
    [Colon] = ":",
    [Comma] = ",",
    [SingleQuote] = "\'",
    [DoubleQuote] = "\"",
    [NewLine] = "\n",
    [Space] = " "
};

enum {
    Assign,
    Plus,
    Minus,
    Multiply,
    Division,
    PlusEqual,
    MinusEqual,
    MultiplyEqual,
    DivisionEqual,
    Or,
    And,
    Xor,
    Not,
    LeftShift,
    RightShift,
    Round,
    Increment,
    Decrement,
    Equal,
    NotEqual,
    GreaterThan,
    LessThan,
    InclusiveGreaterThan,
    InclusiveLessThan,
    LogicalOr,
    LogicalAnd,
    LogicalNot,
};

const char* Operator[] = {
    [Assign] = "=",
    [Plus] = "+",
    [Minus] = "-",
    [Multiply] = "*",
    [Division] = "/",
    [PlusEqual] = "+=",
    [MinusEqual] = "-=",
    [MultiplyEqual] = "*=",
    [DivisionEqual] = "/=",
    [Or] = "|",
    [And] = "&",
    [Xor] = "^",
    [Not] = "~",
    [LeftShift] = "<<",
    [RightShift] = ">>",
    [Round] = "%",
    [Increment] = "++",
    [Decrement] = "--",
    [Equal] = "==",
    [NotEqual] = "!=",
    [GreaterThan] = ">",
    [LessThan] = "<",
    [InclusiveGreaterThan] = ">=",
    [InclusiveLessThan] = "<=",
    [LogicalOr] = "and",
    [LogicalAnd] = "or",
    [LogicalNot] = "!"
};

const char* SingleLineComment[] = {
    "#"
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

void _print_token(token* token, const char* type_name, const char** type_names) {
    printf("(%s, '", type_name);
    for (i32 n = 0; n < token->name_len; ++n) {
        if (token->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(token->name[n]);
    }
    (void)(type_names);
    // if (type_names)
    //     printf("', '%s', %d)\n", type_names[token->name_location], token->name_len);
    // else 
        printf("', %d)\n", token->name_len);
}

void print_token(token* token) {
    switch (token->type) {
    case TokenKeyword: _print_token(token, "keyword", Keyword); break;
    case TokenSeparator: _print_token(token, "separator", Separator); break;
    case TokenOperator: _print_token(token, "operator", Operator); break;
    case TokenLiteral: _print_token(token, "literal", NULL); break;
    case TokenIdentifier: _print_token(token, "identifier", NULL); break;
    case TokenEnd: _print_token(token, "end", NULL); break;
    default: _print_token(token, "unkown", NULL); break;
    }
}

typedef struct {
    i32 type;
    void* info;
    string name;
    i32 name_len;
} object;

hashmap object_map;

object* make_object(object* test_data) {
    object* result = MALLOC(sizeof(object));
    memcpy(result, test_data, sizeof(object));
    return result;
}

size_t hash_object(void* data, size_t size) {
    object* test_data = data;
    printf("hashing %s\n", test_data->name);
    return djb2(test_data->name) % size;
}

void hashmap_free_test_data(void* data) {
    object* test_data = data;
    free_string(&test_data->name);
    // NOTE: not implement info yet
    // FREE(test_data->info);
    FREE(test_data);
}

void print_token_name(token* token) {
    for (i32 n = 0; n < token->name_len; ++n) {
        if (token->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(token->name[n]);
    }
}


INLINE i32 is_data_type(token* token) {
    return token->type == TokenKeyword && token->name_location >= Int && token->name_location <= Char;
}

INLINE i32 is_operator_type(token* token, i32 type) {
    return token->type == TokenOperator && token->name_location == type;
}

INLINE i32 is_variable(token* token) {
    return token->type == TokenIdentifier;
}

INLINE i32 is_separator_type(token* token, i32 type) {
    return token->type == TokenSeparator && token->name_location == type;
}

static i32 in_scope = 0;
void parser_variable(vector(token) tokens, u64 index) {
    i32 assign_offset = 0;
    if (is_separator_type(&tokens[index + 1], Colon)) {
        if (is_data_type(&tokens[index + 2])) {
            if (!in_scope && !is_operator_type(&tokens[index + 3], Assign)) {
                printf("may be missing a '=' ? ");
                return;
            }
            printf("%s ", Keyword[tokens[index + 2].name_location]);
            assign_offset = 2;
        }
        else {
            printf("error type ");
            return;
        }
    }
    if (!in_scope && !is_operator_type(&tokens[index + 1 + assign_offset], Assign)) {
        printf("may be missing a '=' ? ");
        return;
    }

    printf("variable -> ");
    print_token_name(&tokens[index]);
    putchar('\n');
    hashmap_add(object_map, make_object(&(object){
                .info = NULL,
                .type = assign_offset > 0 ? tokens[index + 2].name_location : -1,
                .name_len = tokens[index].name_len,
                .name = make_stringn((string)tokens[index].name, tokens[index].name_len)
            }));
}

void parser_keyword(vector(token) tokens, u64 tokens_len, u64 index) {
    switch (tokens[index].name_location) {
        case Function: {
            if (tokens_len >= 3 && is_variable(&tokens[index + 1])
                && is_separator_type(&tokens[index + 2], OpenRoundBracket)) {
                if (is_separator_type(&tokens[tokens_len - 2], CloseRoundBracket)) {
                    printf("None ");
                }
                printf("function -> ");
                print_token_name(&tokens[index + 1]);
                putchar('\n');
                in_scope = 1;
            }
        } break;
        case If: {
            in_scope = 1;
        } break;
        case End: {
            if (in_scope == 0) {
                printf("end doesn't match any control flow or function!\n");
                exit(1);
            }
            in_scope = 0;
        } break;
        default: break;
    }
}

void parser_identifier(vector(token) tokens, u64 tokens_len, u64 index) {
    if (tokens_len >= 2 && !is_separator_type(&tokens[index + 1], OpenRoundBracket)) {
        parser_variable(tokens, index);
    }
}

void parser_test(vector(token) tokens) {
    u64 len = vector_size(tokens);
    for (u64 i = 0; i < len; ++i) {
        switch (tokens[i].type) {
        case TokenKeyword: {
                parser_keyword(tokens, len, i);
            } break;
        case TokenSeparator: {
            } break;
        case TokenOperator: {
            } break;
        case TokenLiteral: {
            } break;
        case TokenIdentifier: {
                if (tokens[i].type == TokenIdentifier && compare_strings(SingleLineComment, 1, tokens[i].name)) {
                    return;
                }
                parser_identifier(tokens, len, i);
            } break;
        default: break;
        }
    }
    putchar('\n');
}

u64 cal_line_stride(const  char* buffer, i32 line_count) {
    u64 first_n = 0;

    for (i32 i = 0; i < line_count; ++i) {
        first_n = strchr(buffer + first_n, '\n') - buffer + 1;
    }
    return first_n;
}

void command_line_mode(lexer* lexer) {
    u64 line_count = 0, max_line_count = 0;
    u64 begin = 0, end = 0;
    vector(token) tokens = NULL;

    string source_buffer = make_string("");

    while(1) {
        char input[100];
        printf(">");
        fgets(input, sizeof(input), stdin);
        if (input[0] != '.') {
            string_push(source_buffer, input);
            max_line_count++;
            continue;
        }

        if (strlen(input) <= 1) {
            continue;
        }

        switch (input[1]) {
        case 'q': {
            if (tokens) free_vector(&tokens);
            free_string(&source_buffer);
            return;
        }
        case 'b': {
            for (u64 i = 1; i <= max_line_count; ++i) {
                printf("%s", i == line_count ? " -> " : "    ");
                for (u64 j = cal_line_stride(source_buffer, i - 1);
                         j < cal_line_stride(source_buffer, i); ++j) {
                    putchar(source_buffer[j]);
                }
            }
        } break;
        case 'p': if (tokens) parser_test(tokens); break;
        case 'n': {
            begin = cal_line_stride(source_buffer, line_count);
            end = cal_line_stride(source_buffer, line_count + 1);

            ++line_count;
            if (line_count > max_line_count) {
                --line_count;
                printf("no next line\n");
                break;
            }

            if (tokens) free_vector(&tokens);

            tokens = lexer_tokenize_until(lexer, source_buffer + begin, "\n");

            i32 len = end - begin;
            printf("<line:%llu> ", line_count);
            for (i32 c = 0; c < len; ++c) {
                putchar((source_buffer + begin)[c]);
            }
            putchar('\n');
        } break;
        case 'j': {
            if (is_number(input[2])) {
                u64 line = atoi(&input[2]);
                if (line > max_line_count) {
                    printf("out of line\n");
                    break;
                }
                line_count = line;
                if (tokens) free_vector(&tokens);
                begin = cal_line_stride(source_buffer, line_count - 1);
                end = cal_line_stride(source_buffer, line_count);
                tokens = lexer_tokenize_str(lexer, source_buffer + begin, end - begin);
            }
        } break;
        default: break;
        }
    }
    if (tokens) free_vector(&tokens);
    free_string(&source_buffer);
}

void push_object(vector(object)* state, u64 size, void* data, i32 type, const char* name) {
    assert(state != NULL);
    object o = {
        .name = make_string((string)name),
        .type = type,
        .info = MALLOC(size),
        .name_len = strlen(name)
    };
    memcpy(o.info, data, size);

    vector_pushe(*state, o);
}

void pop_object(vector(object)* state) {
    object* o = &(*state)[vector_size(*state) - 1];
    FREE(o->info);
    free_string(&o->name);
    vector_pop(*state);
}

object* get_object(vector(object)* state, const char* name) {
    for (u64 i = 0; i < vector_size(*state); ++i) {
        object* o = &(*state)[i];
        if (strcmp(name, o->name) == 0) {
            return o;
        }
    }
    return NULL;
}

void* get_object_info(vector(object)* state, const char* name) {
    object* o = get_object(state, name);
    return o ? o->info : NULL;
}

i32 main(i32 argc, char** argv) {
    // TEST(stack):

    // NOTE: states[0] can stay global variable
    // vector(vector(object)) states = make_vector();
    // vector_push(states, make_vector());
    //
    // int value = 69;
    // int value1 = 6;
    //
    // push_object(&states[0], sizeof(value), &value, Int, "value");
    // push_object(&states[0], sizeof(value1), &value1, Int, "value1");
    // void* info = get_object_info(&states[0], "value");
    // if (info) {
    //     printf("%d\n", *(int*)info);
    // }
    //
    // info = get_object_info(&states[0], "value1");
    // if (info) {
    //     printf("%d\n", *(int*)info);
    // }
    //
    // pop_object(&states[0]);
    // pop_object(&states[0]);
    //
    // for (u64 i = 0; i < vector_size(states); ++i) {
    //     free_vector(states[i]);
    // }
    //
    // free_vector(states);
    // CHECK_MEMORY_LEAK();
    //
    // return 0;
    // ?init hashmap for variable function name or maybe user define data structure name
    object_map = make_hashmap(1 << 10, hash_object);

    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);

    if (argc == 1) {
        command_line_mode(&lexer);
        exit(0);
    }

    // TODO: to pack source file
    source_file source;
    i32 success = load_source(&source, argv[1], &(token_set){
                .set_name = SingleLineComment,
                .set_size = sizeof(SingleLineComment) / sizeof(void*)
            }, &(token_set){
                .set_name = MultiLineComment,
                .set_size = sizeof(MultiLineComment) / sizeof(void*)
            });

    if (!success) {
        printf("failed load source\n");
        exit(1);
    }

    printf("size = %llu, line count = %llu\n", source.buffer_size, source.line_count);
    printf("----- source begin ------\n%s----- source end -----\n\n", source.buffer);

    u64 offset = 0;
    u64 first_n = 0;
    for (u64 i = 0; i < source.line_count; ++i) {
        first_n = strchr(source.buffer + offset, '\n') - source.buffer - offset + 1;

        vector(token) tokens = lexer_tokenize_until(&lexer, source.buffer + offset, "\n");

        // try match pattern else it's a error
        printf("<line:%llu> ", i + 1);
        for (u64 c = 0; c < first_n; ++c) {
            putchar((source.buffer + offset)[c]);
        }

        parser_test(tokens);
        putchar('\n');

        free_vector(&tokens);
        offset += first_n;
    }

    vector(void*) result = hashmap_access_vector(object_map, &(object){
                    .name = "number"
                });

    for_vector(result, i) {
        object* item = result[i];
        print_token(&(token){
                    .name = item->name,
                    .name_len = item->name_len,
                    .type = TokenIdentifier,
                    .name_location = item->type,
                });
    }

    hashmap_free_items(object_map, hashmap_free_test_data);
    free_hashmap(&object_map);
    free_source(&source);
    CHECK_MEMORY_LEAK();
}

