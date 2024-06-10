#include <stdio.h>
#include <assert.h>

#include "environment.h"
#include "keys_define.h"
#include "interpreter.h"
#include "parser.h"
#include "source_file.h"
#include "string.h"
#include "basic/memallocate.h"
#include "basic/string.h"
#include "lexer.h"

#define LEXER_ADD_TOKEN(lexer, str_set, type)\
    lexer_add_token((lexer), (token_set){\
                .set_name = str_set,\
                .set_size = sizeof(str_set) / sizeof(void*),\
                .token = type\
            }, type);

i32 generate_instructions(char* text, lexer* lex) {
    parser par;
    init_parser(&par);
    char until_ch = '\n';
    int new_line_len = 0, semicolon_len = 0, start_increment = 0;

    i32 line = 1;
    for (i32 start = 0;; ++line) {
        char* ch = NULL;
        ch = strchr(text + start, '\n');
        if (ch) {
            new_line_len = ch - text - start + 1;
            start_increment = new_line_len;
            until_ch = '\n';
        }
        ch = strchr(text + start, ';');
        if (ch) {
            semicolon_len = ch - text - start + 1;
            if (semicolon_len < new_line_len) {
                start_increment = semicolon_len;
                until_ch = ';';
            }
        }

        vector(token) tokens = lexer_tokenize_until(lex, text + start, until_ch);

        if (tokens[0].type == TokenEnd) {
            free_vector(&tokens);
            break;
        }

        if (tokens[0].type == TokenNewLine) {
            free_vector(&tokens);
            start++;
            continue;
        }

        for_vector(tokens, j, 0) {
            tokens[j].line = line;
        }

        start += start_increment;
        start_increment = 0;

        parser_set_tokens(&par, tokens);
        tree_node* node = parser_parse(&par);
        free_vector(&tokens);

        if (!node) {
            continue;
        }

        // if (node->type == NodeVariableInitialize) {
        //     bfs(node, print_node);
        // }
        vector_push(env.inter.instructions, node);
    }

    for_vector(env.error_messages, i, 0) {
        printf("error: %d\n", env.error_messages[i].type);
    }
    return vector_size(env.error_messages);
}

void test(char* test_file) {
    (void)test_file;
    init_environment();
    source_file source;
    i32 success = load_source(&source, test_file);

    if (!success) {
        printf("failed load source\n");
        exit(1);
    }

    lexer lex;
    LEXER_ADD_TOKEN(&lex, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lex, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lex, Operator, TokenOperator);

    if (generate_instructions(source.buffer, &lex) > 0) {
        printf("failed to generate instructions\n");
        free_source(&source);
        delete_environment();
        CHECK_MEMORY_LEAK();
        exit(1);
    }

    parser par;
    init_parser(&par);

    vector_push(env.scopes, make_scope());

    for (; env.inter.index < vector_size(env.inter.instructions); ++env.inter.index) {
        interpret(env.inter.instructions[env.inter.index]);
    }

    free_source(&source);
    delete_environment();
    CHECK_MEMORY_LEAK();
}

// 1st June
// TODO: implement basic string special char e.g. \n \t

void command_line_mode(lexer* lexer);
i32 main(i32 argc, char** argv) {
    // test(argv[1]);
    test("test_expression.cscript");
    test("test_function.cscript");
    return 0;

    init_environment();
    lexer lexer;

    LEXER_ADD_TOKEN(&lexer, Keyword, TokenKeyword);
    LEXER_ADD_TOKEN(&lexer, Separator, TokenSeparator);
    LEXER_ADD_TOKEN(&lexer, Operator, TokenOperator);

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

    printf("size = %u, line count = %u\n", source.buffer_size, source.line_count);
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


