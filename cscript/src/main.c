#include <stdio.h>
#include <assert.h>

#include "environment.h"
#include "keys_define.h"
#include "interpretor.h"
#include "parser.h"
#include "source_file.h"
#include "string.h"
#include "basic/hashmap.h"
#include "basic/memallocate.h"
#include "basic/string.h"
#include "lexer.h"
#include "object.h"

#define LEXER_ADD_TOKEN(lexer, str_set, type)\
    lexer_add_token((lexer), (token_set){\
                .set_name = str_set,\
                .set_size = sizeof(str_set) / sizeof(void*),\
                .token = type\
            }, type);

u64 hash_object(const char* name, i32 len, u64 size) {
    size_t result = 5381;
    for (i32 i = 0; i < len; ++i) {
        result = ((result << 5) + result) + name[i];
    }
    return result % size;
}

object* get_object(const char* name, u64 len) {
    vector(void*) objs = env.object_map.data[hash_object(name, len, env.object_map.size)];
    for (i64 i = vector_size(objs) - 1; i > -1; --i) {
        object* obj = objs[i];
        u64 obj_name = strlen(obj->name);
        if (obj_name == len && strncmp(obj->name, name, len) == 0) {
            return obj;
        }
    }
    return NULL;
}

void evaluate_expression_type(i32* out, tree_node* expression) {
    for_vector(expression->nodes, i, 0) {
        evaluate_expression_type(out, expression->nodes[i]);
    }
    if (expression->type >= NodeDecNumber && expression->type <= NodeBinNumber && expression->object_type > *out) {
        *out = expression->object_type;
    }
}

#define TYPE_CONVERSION(type_a, type_b, value_ptr)\
        {type_a vala = *(type_a*)value_ptr;\
        type_b valb = vala;\
        memcpy(value_ptr, &valb, sizeof(type_b));}

i32 test_check_error(char* text, lexer* lex) {
    i32 error = 0;
    parser par;
    init_parser(&par);
    for (u64 start = 0;;) {
        vector(token) tokens = lexer_tokenize_until(lex, text + start, '\n');
        if (tokens[0].type == TokenEnd) {
            free_vector(&tokens);
            break;
        }

        for_vector(tokens, j, 0) {
            if (tokens[j].type == TokenError) {
                ++error;
            }
        }

        start = strchr(text + start, '\n') - text + 1;

        parser_set_tokens(&par, tokens);
        tree_node* node = parser_parse(&par);
        free_vector(&tokens);

        if (!node) {
            ++error;
            continue;
        }
        dfs(node, free_node);
    }
    free_parser(&par);
    return error;
}

void interpret_variable_initialize(tree_node* node) {
    vector_push(vector_back(env.scopes), make_object(&(object){
            .name = make_stringn(node->name, node->name_len),
            .type = ObjectVariable,
            .info = make_object_variable(&(object_variable){
                .value = MALLOC(sizeof(int)),
                .type = node->object_type
                }),
            }));

    object* obj = vector_back(env.scopes[node->scope_level]);
    printf("scope: %d, %d\n", node->scope_level, node->scope_id);
    hashmap_add(env.object_map, obj);

    i32 expr_type = -1;
    object_variable* info = obj->info;
    evaluate_expression_type(&expr_type, node->nodes[0]->nodes[0]);
    interpret_cal_expression(info->value, expr_type, node->nodes[0]->nodes[0]);

    switch (expr_type - node->object_type) {
        case KeywordInt - KeywordFloat: TYPE_CONVERSION(int, float, info->value); break;
        case KeywordFloat - KeywordInt: TYPE_CONVERSION(float, int, info->value); break;
        default: break;
    }

    switch (info->type) {
        case KeywordInt: printf("%s = %d\n", obj->name, *(i32*)info->value); break;
        case KeywordFloat: printf("%s = %g\n", obj->name, *(f32*)info->value); break;
        default: break;
    }
}

void interpret_variable_assignment(tree_node* node) {
    object* obj = get_object(node->name, node->name_len);
    if (!obj) {
        printf("not found object ");
        print_token_name(&(token){ .name = node->name, .name_len = node->name_len });
        putchar('\n');
        exit(1);
    }

    i32 expr_type = -1;
    object_variable* info = obj->info;
    evaluate_expression_type(&expr_type, node->nodes[0]->nodes[0]);
    interpret_cal_expression(info->value, expr_type, node->nodes[0]->nodes[0]);

    switch (expr_type - node->object_type) {
        case KeywordInt - KeywordFloat: TYPE_CONVERSION(int, float, info->value); break;
        case KeywordFloat - KeywordInt: TYPE_CONVERSION(float, int, info->value); break;
        default: break;
    }

    switch (info->type) {
        case KeywordInt: printf("%s = %d\n", obj->name, *(i32*)info->value); break;
        case KeywordFloat: printf("%s = %g\n", obj->name, *(f32*)info->value); break;
        default: break;
    }
}

void test_interpret(tree_node* node) {
    switch (node->type) {
    case NodeVariableInitialize: interpret_variable_initialize(node); break;
    case NodeVariableAssignment: interpret_variable_assignment(node); break;
    default: printf("not implement node instruction %d yet\n", node->type);  break;
    }
}

void test() {
    // char text[] = "val:float=-((1+2)*(4-1)+(4+2)*(4-1)-(1+2)*(4-1)-(4+2)*(4-1)-1-1-1)*1.5;";
    char text[] = "a:int=1\n"
                  "val:int=1.11*10.0+a\n"
                  "a=13\n"
                  ;

    lexer lex;
    LEXER_ADD_TOKEN(&lex, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lex, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lex, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lex, StringBegin, TokenStringBegin);

    if (test_check_error(text, &lex)) {
        exit(1);
    }
    printf("runnnig\n");

    parser par;
    init_parser(&par);
    vector(tree_node*) instructions = make_vector();

    // test_check_error(text);
    init_environment();
    vector_push(env.scopes, make_scope());

    for (u64 start = 0, line = 1;; ++line) {
        vector(token) tokens = lexer_tokenize_until(&lex, text + start, '\n');
        if (tokens[0].type == TokenEnd) {
            free_vector(&tokens);
            break;
        }

        printf("<line:%llu> ", line);
        for_vector(tokens, j, 0) {
            print_token_name(tokens + j);
            putchar(' ');
        }
        putchar('\n');
        start = strchr(text + start, '\n') - text + 1;

        parser_set_tokens(&par, tokens);
        tree_node* node = parser_parse(&par);
        free_vector(&tokens);

        vector_push(instructions, node);
        test_interpret(node);

        dfs(node, free_node);
    }

    print_parser_error(&par);
    free_parser(&par);
    free_vector(&instructions);

    delete_environment();
    CHECK_MEMORY_LEAK();
}

void command_line_mode(lexer* lexer);
i32 main(i32 argc, char** argv) {
    // {
    //     init_environment();
    //     vector_push(env.scopes, make_scope());
    //     vector_push(vector_back(env.scopes),
    //             .name = make_string("hi"),
    //             .type = 1,
    //             .info = NULL
    //             );
    //     object* val = &vector_back(env.scopes[0]);
    //     hashmap_add(env.object_map, val);
    //     (void)get_object("hi", 2)->name;
    //     delete_environment();
    //     return 0;
    // }
    test();
    return 0;

    init_environment();
    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, StringBegin, TokenStringBegin);

    if (argc == 1) {
        command_line_mode(&lexer);
        delete_environment();
        CHECK_MEMORY_LEAK();
        exit(0);
    }

    // TODO: pack source file
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

        vector(token) tokens = lexer_tokenize_until(&lexer, source.buffer + offset, '\n');

        printf("<line:%llu> ", i + 1);
        for (u64 c = 0; c < first_n; ++c) {
            putchar((source.buffer + offset)[c]);
        }
        for_vector(tokens, i, 0) {
            print_token(tokens + i);
        }
        putchar('\n');

        free_vector(&tokens);
        offset += first_n;
    }

    free_source(&source);
    delete_environment();
    CHECK_MEMORY_LEAK();
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
                parser par = { .index = 0, .tokens = tokens, .tokens_len = vector_size(tokens) };
                tree_node* node = parser_parse(&par);
                if (node) {
                    bfs(node, print_node);
                    dfs(node, free_node);
                }
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
}


