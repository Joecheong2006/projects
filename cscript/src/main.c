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
    KeywordIf,
    KeywordWhile,
    KeywordReturn,
    KeywordConst,
    KeywordInt,
    KeywordFloat,
    KeywordString,
    KeywordChar,
    KeywordTrue,
    KeywordFlase,
    KeywordNone,
    KeywordThen,
    KeywordEnd,
    KeywordFunction,
    KeywordReturnCarry,
    KeywordPower
};

const char* Keyword[] = {
    [KeywordIf] = "if",
    [KeywordWhile] = "while",
    [KeywordReturn] = "return",
    [KeywordConst] = "const",
    [KeywordInt] = "int", 
    [KeywordFloat] = "float", 
    [KeywordChar] = "char",
    [KeywordString] = "string",
    [KeywordTrue] = "None", 
    [KeywordFlase] = "true", 
    [KeywordNone] = "false",
    [KeywordThen] = "then", 
    [KeywordEnd] = "end", 
    [KeywordFunction] = "func", 
    [KeywordReturnCarry] = "->", 
    [KeywordPower] = "**"
};

enum {
    SeparatorOpenSquareBracket,
    SeparatorCloseSquareBracket,
    SeparatorOpenRoundBracket,
    SeparatorCloseRoundBracket,
    SeparatorColon,
    SeparatorComma,
    SeparatorFullStop,
    SeparatorNewLine,
};

// must be in one character
const char* Separator[] = {
    [SeparatorOpenSquareBracket] = "[",
    [SeparatorCloseSquareBracket] = "]",
    [SeparatorOpenRoundBracket] = "(",
    [SeparatorCloseRoundBracket] = ")",
    [SeparatorColon] = ":",
    [SeparatorComma] = ",",
    [SeparatorFullStop] = ".",
    [SeparatorNewLine] = "\n",
};

enum {
    OperatorPlus,
    OperatorMinus,
    OperatorMultiply,
    OperatorDivision,
    OperatorBitwiseOr,
    OperatorBitwiseAnd,
    OperatorBitwiseXor,
    OperatorBitwiseNot,
    OperatorLeftShift,
    OperatorRightShift,

    OperatorAssign,
    OperatorAssignementBegin = OperatorAssign,
    OperatorPlusEqual,
    OperatorMinusEqual,
    OperatorMultiplyEqual,
    OperatorDivisionEqual,
    OperatorBitwiseOrEqual,
    OperatorBitwiseAndEqual,
    OperatorBitwiseXorEqual,
    OperatorBitwiseNotEqual,
    OperatorLeftShiftEqual,
    OperatorRightShiftEqual,
    OperatorRoundEqual,
    OperatorAssignmentEnd = OperatorRoundEqual,

    OperatorRound,
    OperatorIncrement,
    OperatorDecrement,
    OperatorEqual,
    OperatorNotEqual,
    OperatorGreaterThan,
    OperatorLessThan,
    OperatorInclusiveGreaterThan,
    OperatorInclusiveLessThan,
    OperatorOr,
    OperatorAnd,
    OperatorNot,
};

// a = 1;
// a > 1;

const char* Operator[] = {
    [OperatorAssign] = "=",
    [OperatorPlus] = "+",
    [OperatorMinus] = "-",
    [OperatorMultiply] = "*",
    [OperatorDivision] = "/",
    [OperatorPlusEqual] = "+=",
    [OperatorMinusEqual] = "-=",
    [OperatorMultiplyEqual] = "*=",
    [OperatorDivisionEqual] = "/=",
    [OperatorBitwiseOr] = "|",
    [OperatorBitwiseAnd] = "&",
    [OperatorBitwiseXor] = "^",
    [OperatorBitwiseNot] = "~",
    [OperatorBitwiseOrEqual] = "|=",
    [OperatorBitwiseAndEqual] = "&=",
    [OperatorBitwiseXorEqual] = "^=",
    [OperatorBitwiseNotEqual] = "~=",
    [OperatorLeftShift] = "<<",
    [OperatorRightShift] = ">>",
    [OperatorLeftShiftEqual] = "<<=",
    [OperatorRightShiftEqual] = ">>=",
    [OperatorRound] = "%",
    [OperatorRoundEqual] = "%=",
    [OperatorIncrement] = "++",
    [OperatorDecrement] = "--",
    [OperatorEqual] = "==",
    [OperatorNotEqual] = "!=",
    [OperatorGreaterThan] = ">",
    [OperatorLessThan] = "<",
    [OperatorInclusiveGreaterThan] = ">=",
    [OperatorInclusiveLessThan] = "<=",
    [OperatorOr] = "and",
    [OperatorAnd] = "or",
    [OperatorNot] = "!"
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
        temp.name_len -= 2;
        _print_token(&temp, "string literal", NULL);
    } break;
    case TokenIdentifier: _print_token(tok, "identifier", NULL); break;
    case TokenEnd: _print_token(tok, "end", NULL); break;
    case TokenError: _print_token(tok, "error", NULL); break;
    default: _print_token(tok, "unkown", NULL); break;
    }
}

typedef enum {
    DataTypeVariable,
    DataTypeFunction,
} DataType;

typedef struct PACKED {
    i16 type;
    i16 attribute; // NOTE(const readonly writeonly):
    void* value;
} object_variable;

object_variable* make_object_variable() {
    object_variable* variable = MALLOC(sizeof(object_variable));
    *variable = (object_variable){
        .attribute = -1,
        .type = -1,
        .value = NULL,
    };
    return variable;
}

// TODO(object function implementation):
typedef struct {
    i32 type;
} object_function;

typedef struct {
    DataType type;
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

object* get_object(const char* name) {
    vector(void*) objs = hashmap_access_vector(object_map, &(object){ .name = (string)name });
    for_vector(objs, i, 0) {
        object* obj = objs[i];
        if (strcmp(obj->name, name) == 0) {
            return obj;
        }
    }
    return NULL;
}

size_t hash_object(void* data, size_t size) {
    object* test_data = data;
    // printf("hashing %s\n", test_data->name);
    return djb2(test_data->name) % size;
}

void free_object_variable(object* obj) {
    object_variable* var = obj->info;
    FREE(var->value);
    FREE(var);
}

void free_object_function(object* obj) {
    (void)obj;
}

void hashmap_free_test_data(void* data) {
    object* obj = data;
    free_string(&obj->name);
    switch(obj->type) {
    case DataTypeVariable: free_object_variable(obj); break;
    case DataTypeFunction: free_object_function(obj); break;
    default: printf("unkown object"); break;
    }
    FREE(obj);
}

i32 is_string_literal(token* tok);
void print_token_name(token* tok) {
    i32 n = 0, len = tok->name_len;
    if (is_string_literal(tok)) {
        n = 1;
        len -= 1;
    }

    for (; n < len; ++n) {
        if (tok->name[n] == '\n') {
            printf("\\n");
            break;
        }
        putchar(tok->name[n]);
    }
}

INLINE i32 is_identifier(token* tok) { return tok->type == TokenIdentifier; }
INLINE i32 is_keyword(token* tok, i32 type) { return tok->name_location == type; }
INLINE i32 is_operator(token* tok) { return tok->type == TokenOperator; }
INLINE i32 is_separator(token* tok) { return tok->type == TokenSeparator; }

INLINE i32 is_keyword_type(token* tok, i32 type) { return tok->type == TokenKeyword && tok->name_location == type; }
INLINE i32 is_operator_type(token* tok, i32 type) { return tok->type == TokenOperator && tok->name_location == type; }
INLINE i32 is_separator_type(token* tok, i32 type) { return tok->type == TokenSeparator && tok->name_location == type; }
INLINE i32 is_string_literal(token* tok) { return tok->type == TokenStringLiteral; }

INLINE i32 is_data_type(token* tok) { return tok->type == TokenKeyword && tok->name_location >= KeywordInt && tok->name_location <= KeywordChar; }
INLINE i32 is_assigment_operator(token* tok) { return is_operator(tok) && (tok->name_location >= OperatorAssignementBegin && tok->name_location <= OperatorAssignmentEnd); }

i32 is_real_number(token* tok) { 
    if (is_number(tok->name[0] || (tok->name[0] == '.' && is_number(tok->name[1])))) {
        return 1;
    }

    for (i32 i = 0; i < tok->name_len; ++i)
        if (tok->name[i] == '.')
            return 1;
    return 0;
}

// parser implementation
typedef enum {
    ParseErrorNoError,
    ParseErrorMissingToken,
    ParseErrorMissingRhs,
} ParseError;

typedef struct {
    vector(token) tokens;
    u64 index, tokens_len;
    ParseError error;
} parser;

INLINE void set_parse_error(parser* par, i32 error) {
    if (par->error != ParseErrorNoError)
        return;
    par->error = error;
}

INLINE token* parser_peek(parser* par, i32 location) {
    return par->tokens + par->index + location;
}

INLINE token* parser_peekpre(parser* par, i32 location) {
    return par->tokens + par->tokens_len + location;
}

void parse_variable(parser* state) {
    i32 assign_offset = 0;
    if (is_separator_type(parser_peek(state, 1), SeparatorColon)) {
        if (is_data_type(parser_peek(state, 2))) {
            if (!is_operator_type(parser_peek(state, 3), OperatorAssign)) {
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
    if (!is_operator_type(parser_peek(state, assign_offset + 1), OperatorAssign)) {
        printf(" may be missing a '=' ? ");
        return;
    }

    object_variable* variable = make_object_variable();
    if (state->index == 1 && is_keyword_type(state->tokens, KeywordConst)) {
        variable->attribute = KeywordConst;
        printf("const ");
    }

    printf("variable -> ");

    token* identifier = parser_peek(state, 0);
    print_token_name(identifier);

    // printing expression
    for_vector(state->tokens, i, assign_offset + state->index + 1) {
        putchar(' ');
        print_token_name(state->tokens + i);
    }

    token* ex = parser_peek(state, assign_offset + 2);
    if (ex->type == TokenError) {
        printf(" error type");
        FREE(variable);
        return;
    }

    i32 data_type = -1;
    void* data = NULL;
    (void)data;
    if (is_real_number(ex)) {
        data_type = KeywordFloat;
        float number = atof(ex->name);
        data = MALLOC(sizeof(float));
        memcpy(data, &number, sizeof(float));
    }
    else if (is_number(ex->name[0])) {
        data_type = KeywordInt;
        int number = atoi(ex->name);
        data = MALLOC(sizeof(int));
        memcpy(data, &number, sizeof(int));
    }
    else if (is_string_literal(ex)) {
        data_type = KeywordString;
        char* str = MALLOC(ex->name_len - 1);
        memcpy(str, ex->name + 1, ex->name_len - 2);
        str[ex->name_len - 2] = 0;
        data = str;
    }
    else if (is_identifier(ex)) {
        // NOTE:variable expression
    }

    variable->type = assign_offset > 0 ? state->tokens[state->index + 2].name_location : data_type;
    variable->value = data;

    object* obj = make_object(&(object){
                .info = variable,
                .type = DataTypeVariable,
                .name_len = state->tokens[state->index].name_len,
                .name = make_stringn((string)state->tokens[state->index].name, state->tokens[state->index].name_len)
            });

    hashmap_add(object_map, obj);

    putchar('\n');
}

void parse_identifier(parser* state) {
    if (state->index <= 1 && state->tokens_len >= 2 && !is_separator_type(parser_peek(state, 1), SeparatorOpenRoundBracket)) {
        parse_variable(state);
    }
}

void parse_function_parameter(parser* state) {
    i32 assign_offset = 0;
    if (is_separator_type(parser_peek(state, 1), SeparatorColon)) {
        if (is_data_type(parser_peek(state, 2))) {
            printf("%s ", Keyword[parser_peek(state, 2)->name_location]);
            assign_offset = 2;
        }
        else {
            printf("error type ");
            return;
        }
    }

    if (state->index >= 1 && is_keyword_type(parser_peek(state, -1), KeywordConst)) {
        printf("const ");
    }

    if (!is_separator_type(parser_peek(state, assign_offset + 1), SeparatorComma) &&
        !is_separator_type(parser_peek(state, assign_offset + 1), SeparatorCloseRoundBracket)) {
        printf("missing ','");
        return;
    }

    printf("-> ");
    print_token_name(parser_peek(state, 0));
}

void parse_function(parser* state) {
    if (is_separator_type(parser_peekpre(state, -1), SeparatorCloseRoundBracket)) {
        printf("None ");
    }
    printf("function ");
    print_token_name(parser_peek(state, 1));
    printf(" -> param ->");

    for_vector(state->tokens, i, state->index + 2) {
        if (is_identifier(state->tokens + i)) {
            parser new_state = *state;
            new_state.index = i;
            putchar(' ');
            parse_function_parameter(&new_state);
        }
    }

    putchar('\n');
}

static i32 in_scope = 0;
void parser_keyword(parser* state) {
    switch (state->tokens[state->index].name_location) {
        case KeywordFunction: {
            if (state->tokens_len >= 3 && is_identifier(state->tokens + state->index + 1)
                && is_separator_type(state->tokens + state->index + 2, SeparatorOpenRoundBracket)) {
                parse_function(state);
                in_scope = 1;
            }
        } break;
        case KeywordConst: {
            if (state->tokens_len >= 3) {
                parser new_state = *state;
                new_state.index++;
                parse_identifier(&new_state);
            }
        } break;
        case KeywordIf: {
            in_scope = 1;
        } break;
        case KeywordEnd: {
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

    parser state = {
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

void print_object_variable(object* obj) {
    object_variable* var = obj->info;
    switch (var->type) {
        case KeywordInt: printf("%d %s = %d\n", var->attribute, obj->name, *(int*)var->value); break;
        case KeywordFloat: printf("%d %s = %g\n", var->attribute, obj->name, *(float*)var->value); break;
        case KeywordString: printf("%d %s = %s\n", var->attribute, obj->name, (string)var->value); break;
        case KeywordNone: printf("%s = None\n", obj->name); break;
        default: printf("unkown object data type"); break;
    }
}

void print_object(object* obj) {
    switch (obj->type) {
    case DataTypeVariable: print_object_variable(obj); break;
    case DataTypeFunction: {
        printf("not implement print function yet");
    } break;
    default: printf("unkown object data type"); break;
    }
}

// decent parsing implementation
typedef enum {
    NodeVariable,
    NodeOperator,
    NodeNumber,
} Nodetype;

typedef struct tree_node tree_node;
struct tree_node {
    Nodetype type;
    i32 object_type;
    const char* name;
    i32 name_len;
    vector(tree_node*) nodes;
};

tree_node* make_tree_node(Nodetype type, i32 object_type, const char* name, i32 name_len) {
    tree_node* node = MALLOC(sizeof(tree_node));
    memcpy(node, &(tree_node) {
        .type = type,
        .object_type = object_type,
        .name = name,
        .name_len = name_len,
        .nodes = make_vector()
    }, sizeof(tree_node));
    return node;
}

void free_node(tree_node* node) {
    free_vector(&node->nodes);
    FREE(node);
}

void print_node(tree_node* node) {
    printf("node ");
    switch (node->type) {
    case NodeVariable: printf("variable"); break;
    case NodeOperator: printf("operator"); break;
    case NodeNumber: printf("number"); break;
    default: break;
    }
    putchar(' ');
    print_token_name(&(token) {
                .name = node->name,
                .name_len = node->name_len,
            });
    printf(" %d\n", node->object_type);
}

void bfs(tree_node* root, void(*take_action)(tree_node*)) {
    take_action(root);
    for_vector(root->nodes, i, 0) {
        bfs(root->nodes[i], take_action);
    }
}

void dfs(tree_node* root, void(*take_action)(tree_node*)) {
    for_vector(root->nodes, i, 0) {
        dfs(root->nodes[i], take_action);
    }
    take_action(root);
}

void free_tree(tree_node* node) {
    free_node(node);
}

// a = 1 + 3

tree_node* try_parse_identifier(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeVariable, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_number(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeNumber, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_operator(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeOperator, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_round_bracket_pair(parser* state) {
    (void)state;
    return NULL;
}

tree_node* try_parse_expression(parser* par) {
    tree_node* lhs = NULL;
    if (is_identifier(parser_peek(par, 0))) {
        lhs = try_parse_identifier(par);
    }
    else if (is_separator_type(parser_peek(par, 0), SeparatorOpenRoundBracket)) {
        ++par->index;
        try_parse_round_bracket_pair(par);
    }
    else if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
        lhs = try_parse_number(par);
    }
    else {
        return NULL;
    }

    ++par->index;
    tree_node* operator = NULL;

    if (is_separator_type(parser_peek(par, 0), SeparatorCloseRoundBracket) ||
        par->index == par->tokens_len) {
        return lhs;
    }

    if (is_operator(parser_peek(par, 0))) {
        operator = try_parse_operator(par);
    }

    if (!operator) {
        free_node(lhs);
        set_parse_error(par, ParseErrorMissingToken);
        return NULL;
    }

    vector_pushe(operator->nodes, lhs);
    ++par->index;
    tree_node* rhs = try_parse_expression(par);
    if (!rhs) {
        set_parse_error(par, ParseErrorMissingRhs);
        free_node(lhs);
        free_node(operator);
        return NULL;
    }
    vector_pushe(operator->nodes, rhs);
    return operator;
}

void try_parse_variable_assignment(parser* state) {
    (void)state;
}

tree_node* try_parse_variable(parser* par) {
    token* var_tok = parser_peek(par, 0);
    tree_node* var = make_tree_node(NodeVariable, -1, var_tok->name, var_tok->name_len);
    if (is_assigment_operator(parser_peek(par, 1))) {
        token* assign_tok = parser_peek(par, 1);
        tree_node* assign_operator = make_tree_node(NodeOperator, assign_tok->name_location, assign_tok->name, assign_tok->name_len);
        par->index += 2;
        tree_node* expression = try_parse_expression(par);
        if (!expression) {
            free_node(var);
            free_node(assign_operator);
            return NULL;
        }
        vector_pushe(var->nodes, assign_operator);
        vector_pushe(var->nodes[0]->nodes, expression);
    }
    return var;
}

tree_node* parse_test(parser* par) {
    switch (par->tokens[0].type) {
    case TokenIdentifier: {
        return try_parse_variable(par);
    } break;
    default:
        break;
    }
    return NULL;
}

i32 main(i32 argc, char** argv) {
    object_map = make_hashmap(1 << 10, hash_object);

    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, StringBegin, TokenStringBegin);
    
    {
        /*          val
         *           |
         *           =
         *           |
         *           -
         *          / \
         *        1.2  +
         *            / \
         *           2   3
         *
         *
         */
        const char text[] = "val-=1.2-.2+3\n";
        vector(token) tokens = lexer_tokenize_until(&lexer, text, '\n');
        for_vector(tokens, i, 0) {
            print_token_name(tokens + i);
            putchar(' ');
        }
        putchar('\n');

        parser par = {
            .index = 0,
            .tokens = tokens,
            .tokens_len = vector_size(tokens),
            .error = ParseErrorNoError,
        };
        tree_node* node = parse_test(&par);
        if (node) {
            bfs(node, print_node);
            dfs(node, free_tree);
        }
        printf("error %d\n", par.error);

        free_vector(&tokens);
        hashmap_free_items(object_map, hashmap_free_test_data);
        free_hashmap(&object_map);
        CHECK_MEMORY_LEAK();
        return 0;
    }
   
    {
        const char text[] = "const val=1.2\n";
        vector(token) tokens = lexer_tokenize_until(&lexer, text, '\n');
        parser_test(tokens);

        object* obj = get_object("val");
        if (obj)
            print_object(obj);

        free_vector(&tokens);
        hashmap_free_items(object_map, hashmap_free_test_data);
        free_hashmap(&object_map);
        CHECK_MEMORY_LEAK();
        return 0;
    }

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

