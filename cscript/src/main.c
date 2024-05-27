#include <stdio.h>

#include "keys_define.h"
#include "interpretor.h"
#include "source_file.h"
#include "string.h"
#include "basic/hashmap.h"
#include "basic/memallocate.h"
#include "basic/string.h"
#include "lexer.h"

#define LEXER_ADD_TOKEN(lexer, str_set, type)\
    lexer_add_token((lexer), (token_set){\
                .set_name = str_set,\
                .set_size = sizeof(str_set) / sizeof(void*),\
                .token = type\
            }, type);

typedef enum {
    ObjectVariable,
    ObjectFunction,
} ObjectType;

typedef struct {
    void* info;
    string name;
    ObjectType type;
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

void free_object(void* data) {
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
                    dfs(node, free_node);
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

void get_expression_data_type(KeywordType* out, tree_node* expression) {
    for_vector(expression->nodes, i, 0) {
        get_expression_data_type(out, expression->nodes[i]);
    }
    if (expression->type == NodeDecNumber && expression->object_type > (i32)*out) {
        *out = expression->object_type;
    }
}

void construct_object(object* obj);
void destruct_object(object* obj);

void construct_object(object* obj) {
    (void)obj;
    // NOTE: call constrcutor if necessary
}

void destruct_object(object* obj) {
    (void)obj;
    // NOTE: call destrcutor if necessary
}

typedef vector(object) scope;
void init_scope(scope* s);
void scope_push(scope* s, object* obj);
void scope_pop(scope* s);
void free_scope(scope* s);

scope make_scope() {
    return make_vector();
}

void scope_push(scope* s, object* obj) {
    vector_pushe(*s, *obj);
    construct_object(obj);
}

void scope_pop(scope* s) {
    object* obj = &vector_back(*s);
    vector_pop(*s);
    destruct_object(obj);
}

void free_scope(scope* s) {
    for_vector(*s, i, 0) {
        FREE((*s)[i].info); // need to call specif func
        free_string(&(*s)[i].name);
    }
    free_vector(s);
}

// TODO: checking lexer error from parser
void test(lexer* lex) {
    vector(scope) scopes = make_vector();
    vector(tree_node*) instructions = make_vector();

    char text[] = "val:float=-((1+2)*(4-1)+(4+2)*(4-1)-(1+2)*(4-1)-(4+2)*(4-1)-1-1-1)*1.5;";
    // const char text[] = "val=0xabcdef * (0b1010 * 0o7) * 0.01\n";
    vector(token) tokens = lexer_tokenize_until(lex, text, '\n');

    for_vector(tokens, i, 0) {
        print_token_name(tokens + i);
        putchar(' ');
    }
    putchar('\n');

    parser par;
    init_parser(&par, tokens);

    tree_node* node = parser_parse(&par);
    if (node) {
        void* expr_result = MALLOC(sizeof(int));
        memset(expr_result, 0, sizeof(int));
        interpret_cal_expression(expr_result, node->object_type, node->nodes[0]->nodes[0]);

        vector_push(instructions, node);
        vector_push(scopes, make_scope());

        struct variable_info {
            void* value;
            i32 type; // Keyword
        };

        vector_push(vector_back(scopes),
                    .name = make_stringn(instructions[0]->name, instructions[0]->name_len),
                    .type = ObjectVariable,
                    .info = MALLOC(sizeof(struct variable_info)),
                );

        object* val = &scopes[instructions[0]->scope_level][instructions[0]->scope_id];
        struct variable_info* info = val->info;
        info->value = expr_result;
        info->type = instructions[0]->object_type;

        switch (info->type) {
        case KeywordInt: printf("%s = %d\n", val->name, *(i32*)info->value); break;
        case KeywordFloat: printf("%s = %g\n", val->name, *(f32*)info->value); break;
        default: break;
        }

        FREE(expr_result);

        dfs(node, free_node);
    }

    printf("error %d\n", par.error);
    free_vector(&tokens);
    free_vector(&instructions);

    for_vector(scopes, i, 0) {
        free_scope(&scopes[i]);
    }
    free_vector(&scopes);

    hashmap_free_items(object_map, free_object);
    free_hashmap(&object_map);
    CHECK_MEMORY_LEAK();
}

i32 main(i32 argc, char** argv) {
    object_map = make_hashmap(1 << 10, hash_object);

    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);
    LEXER_ADD_TOKEN(&lexer, StringBegin, TokenStringBegin);
    //
    // test(&lexer);
    // return 0;
    //
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

    hashmap_free_items(object_map, free_object);
    free_hashmap(&object_map);
    free_source(&source);
    CHECK_MEMORY_LEAK();
}

