#include "platform/platform.h"
#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "interpreter.h"
#include "parser.h"
#include "core/assert.h"

#include "environment.h"
#include "tracing.h"

__attribute__((destructor(101)))
static void check_leak(void) {
    LOG_DEBUG("\tleak count = %d\n", check_memory_leak());
}

vector(command*) gen_instructions(vector(ast_node*) ast) {
    START_PROFILING();
    vector(command*) ins = make_vector(const command*);
    for_vector(ast, i, 0) {
        command* cmd = ast[i]->gen_command(ast[i]);
        vector_push(ins, cmd);
    }
    END_PROFILING(__func__);
    return ins;
}

int main(void) {
    platform_state state;
    setup_platform(&state);
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3.0+1;";
    const char text[] = "fun add(a, b)\n"
                        "\tvar c=a+b\n"
                        "\tadd(a, b)\n"
                        "end\n"
                        "add(1-1-1--3*3.0+1, 1.0 + 101 % 3 / 2.0)\n"
                        ;
    // const char text[] = "func(1-1-1--3*3.0+1, 1.0 + 101 % 3 / 2.0)()(1.)(1.,2.)(1.,2.,3.)(1.,2.,3.,4.)(1.,2.,3.,4.,5.)(1.,2.,3.,4.,5.,6.)(1.,2.,3.,4.,5.,6.,7.)\n";
    // const char text[] = "var a= (2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5)+(.5-0.3-0.2)\n"
    //                     "var cat = 1-1.0-1--3*3\n"
    //                     "var dog = 1.0 + 101 % 3 / 2\n"
    //                     "dog += 3.1415\n"
    //                     "var v = dog + cat\n"
    //                     "v -= dog\n";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    init_parser(&par, generate_tokens(&lex));

    vector(ast_node*) ast = parser_parse(&par);
    for_vector(par.errors, i, 0) {
        LOG_ERROR("\t%d:%d %s\n", par.errors[i].line, par.errors[i].position, par.errors[i].msg);
    }

    if (ast) {
        interpreter inter;
        init_interpreter(&inter, gen_instructions(ast));
        free_ast(ast);
        free_parser(&par);

        env_push_scope(&inter.env);
        for_vector(inter.ins, i, 0) {
            error_info ei = interpret_command(&inter);
            if (ei.msg) {
                LOG_ERROR("\t%s on line %d\n", ei.msg, ei.line);
                ASSERT_MSG(0, "failed to exec command");
            }
        }
        env_pop_scope(&inter.env);

        free_interpreter(&inter);
    }
    else {
        free_parser(&par);
    }
    // free_vector(lex.ctx);
    shutdown_platform(&state);

    return 0;
}

