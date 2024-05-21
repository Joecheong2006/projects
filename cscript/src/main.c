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
    [Function] = "func", 
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
    FullStop,
    NewLine,
};

// must be in one character
const char* Separator[] = {
    [OpenSquareBracket] = "[",
    [CloseSquareBracket] = "]",
    [OpenRoundBracket] = "(",
    [CloseRoundBracket] = ")",
    [Colon] = ":",
    [Comma] = ",",
    [FullStop] = ".",
    [NewLine] = "\n",
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

enum {
    DoubleQuote,
    SingleQuote,
};

const char* StringBegin[] = {
    [DoubleQuote] = "\"",
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

void print_token(token* tok) {
    switch (tok->type) {
    case TokenKeyword: _print_token(tok, "keyword", Keyword); break;
    case TokenSeparator: _print_token(tok, "separator", Separator); break;
    case TokenOperator: _print_token(tok, "operator", Operator); break;
    case TokenLiteral: _print_token(tok, "literal", NULL); break;
    case TokenStringLiteral: {
        token temp = *tok;
        temp.name += 1;
        _print_token(&temp, "string literal", NULL);
    } break;
    case TokenIdentifier: _print_token(tok, "identifier", NULL); break;
    case TokenEnd: _print_token(tok, "end", NULL); break;
    case TokenError: _print_token(tok, "error", NULL); break;
    default: _print_token(tok, "unkown", NULL); break;
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
    // printf("hashing %s\n", test_data->name);
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

// parser implementation
typedef struct {
    vector(token) tokens;
    u64 index, tokens_len;
} parse_state;

INLINE token* parser_peek(parse_state* state, i32 location) {
    return state->tokens + state->index + location;
}

INLINE token* parser_peekpre(parse_state* state, i32 location) {
    return state->tokens + state->tokens_len + location;
}

INLINE i32 is_variable(token* tok) { return tok->type == TokenIdentifier; }
INLINE i32 is_keyword(token* tok, i32 type) { return tok->name_location == type; }
INLINE i32 is_operator(token* tok) { return tok->type == TokenOperator; }
INLINE i32 is_separator(token* tok) { return tok->type == TokenSeparator; }

INLINE i32 is_keyword_type(token* tok, i32 type) { return tok->type == TokenKeyword && tok->name_location == type; }
INLINE i32 is_data_type(token* tok) { return tok->type == TokenKeyword && tok->name_location >= Int && tok->name_location <= Char; }
INLINE i32 is_operator_type(token* tok, i32 type) { return tok->type == TokenOperator && tok->name_location == type; }
INLINE i32 is_separator_type(token* tok, i32 type) { return tok->type == TokenSeparator && tok->name_location == type; }

void parse_variable(parse_state* state) {
    i32 assign_offset = 0;
    if (is_separator_type(parser_peek(state, 1), Colon)) {
        if (is_data_type(parser_peek(state, 2))) {
            if (!is_operator_type(parser_peek(state, 3), Assign)) {
                printf(" may be missing a '=' ? ");
                return;
            }
            printf("%s ", Keyword[parser_peek(state, 2)->name_location]);
            assign_offset = 2;
        }
        else {
            printf("error type ");
            return;
        }
    }
    if (!is_operator_type(parser_peek(state, assign_offset + 1), Assign)) {
        printf(" may be missing a '=' ? ");
        return;
    }

    if (state->index == 1 && is_keyword_type(state->tokens, Const)) {
        printf("const ");
    }

    printf("variable -> ");
    print_token_name(parser_peek(state, 0));

    // printing expression
    for_vector(state->tokens, i, assign_offset + state->index + 1) {
        putchar(' ');
        print_token_name(state->tokens + i);
    }

    putchar('\n');
    hashmap_add(object_map, make_object(&(object){
                .info = NULL,
                .type = assign_offset > 0 ? state->tokens[state->index + 2].name_location : -1,
                .name_len = state->tokens[state->index].name_len,
                .name = make_stringn((string)state->tokens[state->index].name, state->tokens[state->index].name_len)
            }));
}

void parse_identifier(parse_state* state) {
    if (state->index <= 1 && state->tokens_len >= 2 && !is_separator_type(parser_peek(state, 1), OpenRoundBracket)) {
        parse_variable(state);
    }
}

void parse_function_parameter(parse_state* state) {
    i32 assign_offset = 0;
    if (is_separator_type(parser_peek(state, 1), Colon)) {
        if (is_data_type(parser_peek(state, 2))) {
            printf("%s ", Keyword[parser_peek(state, 2)->name_location]);
            assign_offset = 2;
        }
        else {
            printf("error type ");
            return;
        }
    }

    if (state->index >= 1 && is_keyword_type(parser_peek(state, -1), Const)) {
        printf("const ");
    }

    if (!is_separator_type(parser_peek(state, assign_offset + 1), Comma) &&
        !is_separator_type(parser_peek(state, assign_offset + 1), CloseRoundBracket)) {
        printf("missing ','");
        return;
    }

    printf("-> ");
    print_token_name(parser_peek(state, 0));
}

void parse_function(parse_state* state) {
    if (is_separator_type(parser_peekpre(state, -1), CloseRoundBracket)) {
        printf("None ");
    }
    printf("function ");
    print_token_name(parser_peek(state, 1));
    printf(" -> param ->");

    for_vector(state->tokens, i, state->index + 2) {
        if (is_variable(state->tokens + i)) {
            parse_state new_state = *state;
            new_state.index = i;
            putchar(' ');
            parse_function_parameter(&new_state);
        }
    }

    putchar('\n');
}

static i32 in_scope = 0;
void parser_keyword(parse_state* state) {
    switch (state->tokens[state->index].name_location) {
        case Function: {
            if (state->tokens_len >= 3 && is_variable(state->tokens + state->index + 1)
                && is_separator_type(state->tokens + state->index + 2, OpenRoundBracket)) {
                parse_function(state);
                in_scope = 1;
            }
        } break;
        case Const: {
            if (state->tokens_len >= 3) {
                parse_state new_state = *state;
                new_state.index++;
                parse_identifier(&new_state);
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

void parser_test(vector(token) tokens) {

    parse_state state = {
        .tokens = tokens,
        .tokens_len = vector_size(tokens)
    };

    for (u64 i = 0; i < 1; ++i) {
        state.index = i;
        switch (tokens[i].type) {
        case TokenKeyword: {
                parser_keyword(&state);
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
                parse_identifier(&state);
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

            tokens = lexer_tokenize_until(lexer, source_buffer + begin, '\n');

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

enum {
    NodePlus,
    NodeMinus,
    NodeMultiply,
    NodeData,
};

typedef struct nodeI nodeI;
struct nodeI {
    i32 type;
    int data;
    nodeI* lhs;
    nodeI* rhs;
    int(*cal)(nodeI*);
};

int node_data_calI(nodeI* n) {
    return n->data;
}

int node_multiply_calI(nodeI* n) {
    return n->lhs->cal(n->lhs) * n->rhs->cal(n->rhs);
}

int node_plus_calI(nodeI* n) {
    return n->lhs->cal(n->lhs) + n->rhs->cal(n->rhs);
}

INLINE nodeI init_node_dataI(int data) { return (nodeI){ .cal = node_data_calI, .type = NodeData, .data = data }; }
INLINE nodeI init_node_plusI(nodeI* lhs, nodeI* rhs) { return (nodeI){ .cal = node_plus_calI, .type = NodePlus, .lhs = lhs, .rhs = rhs }; }
INLINE nodeI init_node_multiplyI(nodeI* lhs, nodeI* rhs) { return (nodeI){ .cal = node_multiply_calI, .type = NodeMultiply, .lhs = lhs, .rhs = rhs }; }

int cal_expressionI(nodeI* root) {
    return root->cal(root);
}

i32 main(i32 argc, char** argv) {
    object_map = make_hashmap(1 << 10, hash_object);

    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, StringBegin, TokenStringBegin);

    /*
    {
            //    + 
            //   / \
            //  4   *
            //     / \
            //    2   3
            // -> 2 * 3 + 4 = 10

        nodeI node4  = init_node_dataI(4);
        nodeI node3 = init_node_dataI(3);
        nodeI node2 = init_node_dataI(2);

        nodeI mult = init_node_multiplyI(&node2, &node3);
        nodeI plus = init_node_plusI(&node4, &mult);

        printf("%d\n", cal_expressionI(&plus));

    } return 0;
    */
    {
        const char text[] = "str=\"hi\"\n";
        vector(token) tokens = lexer_tokenize_until(&lexer, text, '\n');
        for_vector(tokens, i, 0) {
            print_token(tokens + i);
        }
        // parser_test(tokens);
        free_vector(&tokens);
    } return 0;

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

        vector(token) tokens = lexer_tokenize_until(&lexer, source.buffer + offset, '\n');

        // try match pattern else it's a error
        printf("<line:%llu> ", i + 1);
        for (u64 c = 0; c < first_n; ++c) {
            putchar((source.buffer + offset)[c]);
        }
        for_vector(tokens, i, 0) {
            print_token(tokens + i);
        }

        // parser_test(tokens);
        putchar('\n');

        free_vector(&tokens);
        offset += first_n;
    }

    vector(void*) result = hashmap_access_vector(object_map, &(object){
                    .name = "number"
                });

    for_vector(result, i, 0) {
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

