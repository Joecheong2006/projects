#include <string.h>
#include <assert.h>
#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"
#include "parser.h"

#include "object.h"

typedef vector(object*) scope;

void free_scope(scope sc) {
    for_vector(sc, i, 0) {
        free_object(sc[i]);
    }
    free_vector(sc);
}

int main(void) {
    // {
    //     scope sc = make_vector(object*);
    //     free_scope(sc);
    //     printf("leak count = %d\n", check_memory_leak());
    // }
    // return 0;
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3.0+1";
    const char text[] = "var a=(2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5)+(.5-0.3-0.2)\n"
                        "var cat = 1-1.0-1--3*3";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    vector(ast_node*) ins = parser_parse(&par);
    if (ins) {
        for_vector(ins, i, 0) {
            command* cmd = ins[i]->gen_command(ins[i]);
            assert(cmd->exec(NULL, cmd) == cmd);
            cmd->destroy(cmd);
        }
        for_vector(ins, i, 0) {
            ast_tree_free(ins[i]);
        }
        free_vector(ins);
    }

    for_vector(par.errors, i, 0) {
        LOG_ERROR("\t%d:%d %s\n", par.errors[i].tok->line, par.errors[i].tok->count, par.errors[i].msg);
    }

    parser_free(&par);
    // free_vector(lex.ctx);

    LOG_INFO("\tleak count = %d\n", check_memory_leak());
    return 0;
}
