#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"
#include "parser.h"
#include "core/assert.h"
#include "tracing.h"

#include "global.h"

int main(void) {
    // test();
    // return 0;
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3.0+1;";
    const char text[] = "var a=(2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5)+(.5-0.3-0.2)\n"
                        "var cat = 1-1.0-1--3*3\n"
                        "var dog = 1.0 + 101 % 3 / 2\n"
                        "dog += 1.3\n";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    vector(ast_node*) ast = parser_parse(&par);
    if (ast) {
        setup_global_env();
        scopes_push();

        vector(command*) ins = make_vector(command*);
        for_vector(ast, i, 0) {
            command* cmd = ast[i]->gen_command(ast[i]);
            vector_push(ins, cmd);
        }

        for_vector(ins, i, 0) {
            ASSERT(exec_command(ins[i]));
        }

        for_vector(ins, i, 0) {
            ins[i]->destroy(ins[i]);
        }
        free_vector(ins);

        scopes_pop();
        shutdown_global_env();

        for_vector(ast, i, 0) {
            ast[i]->destroy(ast[i]);
        }
        free_vector(ast);
    }

    for_vector(par.errors, i, 0) {
        LOG_ERROR("\t%d:%d %s\n", par.errors[i].tok->line, par.errors[i].tok->count, par.errors[i].msg);
    }

    parser_free(&par);
    // free_vector(lex.ctx);

    LOG_INFO("\tleak count = %d\n", check_memory_leak());
    return 0;
}
