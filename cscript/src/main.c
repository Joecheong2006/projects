#include <stdio.h>
#include <string.h>

#include "keys_define.h"
#include "parser.h"
#include "source_file.h"
#include "string.h"
#include "hashmap.h"
#include "memallocate.h"
#include "lexer.h"
#include "util.h"

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
    printf("', %d)\n", token->name_len);
}

void print_token(token* tok) {
    switch (tok->type) {
    case TokenOpenBrace:
    case TokenCloseBrace:
    case TokenOpenSquareBracket:
    case TokenCloseSquareBracket:
    case TokenOpenRoundBracket:
    case TokenCloseRoundBracket:
    case TokenSemicolon:
    case TokenComma:
    case TokenFullStop:
    case TokenNewLine:
                   _print_token(tok, "default separator", NULL); break;
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

typedef struct {
    void* info;
    string name;
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
    return djb2(test_data->name) % size;
}

void hashmap_free_test_data(void* data) {
    object* obj = data;
    free_string(&obj->name);
    // NOTE: free obj->info
    FREE(obj);
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
        case 'p': {
            if (tokens) {
                parser par = { .index = 0, .tokens = tokens, .tokens_len = vector_size(tokens), .error = ParseErrorNoError };
                tree_node* node = parser_parse(&par);
                if (node) {
                    bfs(node, print_node);
                    dfs(node, free_tree);
                }
                printf("error %d\n", par.error);
            }
        } break;
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
                tokens = lexer_tokenize_until(lexer, source_buffer + begin, '\n');
            }
        } break;
        default: break;
        }
    }
    if (tokens) free_vector(&tokens);
    free_string(&source_buffer);
    CHECK_MEMORY_LEAK();
}

void* interpret(tree_node* instruction);
void interpret_node_variable(tree_node* node) {
    (void)node;
    interpret(node->nodes[0]);
}

int get_node_number_value_int(tree_node* node) {
    return atoi(node->name);
}

float get_node_number_value_float(tree_node* node) {
    return atof(node->name);
}

#define INTERPRET_OPERATE_NUMBERS(type, data, operator)\
        type lhs_value = get_node_number_value_##type(lhs);\
        type rhs_value = get_node_number_value_##type(rhs);\
        type sum = lhs_value operator rhs_value + *(type*)out;\
        memcpy(data, &sum, sizeof(type));

INLINE void interpret_operator_plus_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs + *rhs;
    memcpy(out, &sum, sizeof(int));
}

INLINE void interpret_operator_minus_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs - *rhs;
    memcpy(out, &sum, sizeof(int));
}

INLINE void interpret_operator_multiply_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs * *rhs;
    memcpy(out, &sum, sizeof(int));
}

INLINE void interpret_operator_division_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs / *rhs;
    memcpy(out, &sum, sizeof(int));
}

INLINE void interpret_operator_plus_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs + *rhs;
    memcpy(out, &sum, sizeof(float));
}

INLINE void interpret_operator_minus_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs - *rhs;
    memcpy(out, &sum, sizeof(float));
}

INLINE void interpret_operator_multiply_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs * *rhs;
    memcpy(out, &sum, sizeof(float));
}

INLINE void interpret_operator_division_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs / *rhs;
    memcpy(out, &sum, sizeof(float));
}

void interpret_operator_arithmetic_int(OperatorType type, int* out, int* lhs, int* rhs) {
    switch (type) {
    case OperatorPlus: interpret_operator_plus_int(out, lhs, rhs); break;
    case OperatorMinus: interpret_operator_minus_int(out, lhs, rhs); break;
    case OperatorMultiply: interpret_operator_multiply_int(out, lhs, rhs); break;
    case OperatorDivision: interpret_operator_division_int(out, lhs, rhs); break;
    default: break;
    }
}

void interpret_operator_arithmetic_float(OperatorType type, float* out, float* lhs, float* rhs) {
    switch (type) {
    case OperatorPlus: interpret_operator_plus_float(out, lhs, rhs); break;
    case OperatorMinus: interpret_operator_minus_float(out, lhs, rhs); break;
    case OperatorMultiply: interpret_operator_multiply_float(out, lhs, rhs); break;
    case OperatorDivision: interpret_operator_division_float(out, lhs, rhs); break;
    default: break;
    }
}

void interpret_cal_expression_int(int* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        int lhs, rhs;
        interpret_cal_expression_int(&lhs, node->nodes[0]);
        interpret_cal_expression_int(&rhs, node->nodes[1]);
        interpret_operator_arithmetic_int(node->object_type, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: {
        interpret_cal_expression_int(out, node->nodes[0]);
        *out = -*out;
    } break;
    case NodeNumber: {
        int value = get_node_number_value_int(node);
        memcpy(out, &value, sizeof(int));
    } break;
    default: break;
    }
}

void interpret_cal_expression_float(float* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        float lhs, rhs;
        interpret_cal_expression_float(&lhs, node->nodes[0]);
        interpret_cal_expression_float(&rhs, node->nodes[1]);
        interpret_operator_arithmetic_float(node->object_type, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: {
        interpret_cal_expression_float(out, node->nodes[0]);
        *out = -*out;
    } break;
    case NodeNumber: {
        float value = get_node_number_value_float(node);
        memcpy(out, &value, sizeof(float));
    }break;
    default: break;
    }
}

void interpret_cal_expression(void* out, KeywordType data_type, tree_node* node) {
    switch (data_type) {
    case KeywordInt: {
        interpret_cal_expression_int(out, node);
    } break;
    case KeywordFloat: {
        interpret_cal_expression_float(out, node);
    } break;
    default: {
    } break;
    }
}

void get_expression_data_type(KeywordType* out, tree_node* expression) {
    for_vector(expression->nodes, i, 0) {
        get_expression_data_type(out, expression->nodes[i]);
    }
    if (expression->type == NodeNumber && expression->object_type > (i32)*out) {
        *out = expression->object_type;
    }
}

void* interpret(tree_node* instruction) {
    switch (instruction->type) {
    case NodeVariableAssign: break;
    case NodeOperator: break;
    case NodeAssignmentOperator: break;
    case NodeNumber: break;
    default: break;
    };
    return NULL;
}

i32 main(i32 argc, char** argv) {
    object_map = make_hashmap(1 << 10, hash_object);

    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, StringBegin, TokenStringBegin);
    
#if 1
    // {
    //     char text[] = "...>>:=(1[2{3}2]1)0xabcdef(1)\n";
    //     vector(token) tokens = lexer_tokenize_test(&lexer, text, '\n');
    //     for_vector(tokens, i, 0) {
    //         print_token(tokens + i);
    //     }
    //     free_vector(&tokens);
    //     return 0;
    // }
    {
        // TODO: implement base16 and base8 and base2 transformation
        const char text[] = "val=-((1+2)*(4-1)-(1+2)*(4-1)+(1+2)*(4-1)-(1+2)*(4-1)-1)\n";
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
        tree_node* node = parser_parse(&par);
        if (node) {
            // bfs(node, print_node);
            // putchar('\n');

            KeywordType type = 0;
            get_expression_data_type(&type, node->nodes[0]->nodes[0]);

            void* expr_result = MALLOC(sizeof(int));
            memset(expr_result, 0, 4);
            interpret_cal_expression(expr_result, type, node->nodes[0]->nodes[0]);

            switch (type) {
            case KeywordInt: printf("expression %d\n", *(int*)expr_result); break;
            case KeywordFloat: printf("expression %g\n", *(float*)expr_result); break;
            default: break;
            }
            FREE(expr_result);

            printf("%d\n", type);

            dfs(node, free_tree);
        }
        printf("error %d\n", par.error);

        free_vector(&tokens);
        hashmap_free_items(object_map, hashmap_free_test_data);
        free_hashmap(&object_map);
        CHECK_MEMORY_LEAK();
        return 0;
    }
#endif

    if (argc == 1) {
        command_line_mode(&lexer);
        exit(0);
    }

    // TODO: to pack source file
    source_file source;
    i32 success = load_source(&source, argv[1]);

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

        // vector(token) tokens = lexer_tokenize_until(&lexer, source.buffer + offset, '\n');
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
                    .type = TokenIdentifier,
                });
    }

    hashmap_free_items(object_map, hashmap_free_test_data);
    free_hashmap(&object_map);
    free_source(&source);
    CHECK_MEMORY_LEAK();
}

