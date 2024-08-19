#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"
#include "parser.h"

#include "object.h"

// TODO(Aug17): create scope 

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

    for_vector(par.tokens, i, 0) {
        char buf[100];
        sprintf(buf, "%d:%d:%d ", par.tokens[i].line, par.tokens[i].count, par.tokens[i].type);
        printf("%s %*c", buf, 14 - (i32)strlen(buf), ' ');
        if (par.tokens[i].type == TokenTypeLiteralInt32) {
            printf("i32: %d\n", par.tokens[i].val.int32);
        }
        else if (par.tokens[i].type == TokenTypeLiteralFloat32) {
            printf("f32: %g\n", par.tokens[i].val.float32);
        }
        else if (par.tokens[i].type == TokenTypeIdentifier) {
            printf("id:  %s len: %d\n", par.tokens[i].val.string, (i32)strlen(par.tokens[i].val.string));
        }
        else {
            if (par.tokens[i].type < 256)
                printf("sym: '%c' asc: %d\n", par.tokens[i].type == '\n' ? ' ' : par.tokens[i].type, par.tokens[i].type);
            else
                printf("key: %s\n", TokenTypeString[par.tokens[i].type - 256]);
        }
    }

    vector(ast_node*) ins = parser_parse(&par);
    if (ins) {
        for_vector(ins, i, 0) {
            command* cmd = ins[i]->gen_command(ins[i]);
            assert(cmd->exec(NULL, cmd) == cmd);
            cmd->destroy(cmd);

            // printf("%s = %g\n", cmd->arg1->name, cmd->arg2->data->float32);
            // free_string(cmd->arg1->name);

            //free_command(cmd);
        }
        for_vector(ins, i, 0) {
            ast_tree_free(ins[i]);
        }
        free_vector(ins);
    }

    for_vector(par.errors, i, 0) {
        printf("%d:%d %s\n", par.errors[i].tok->line, par.errors[i].tok->count, par.errors[i].msg);
    }

    parser_free(&par);
    // free_vector(lex.ctx);

    LOG_INFO("\tleak count = %d\n", check_memory_leak());
    return 0;
}
