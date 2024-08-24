#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"
#include "parser.h"
#include "core/assert.h"

#include "object.h"
#include "global.h"

void test(void) {
    setup_global_env();
    scopes_push();

    {
        object* obj = make_object_bool("bool");
        object_bool* b = get_object_true_type(obj);
        b->val = 1;
        push_object(obj);
    }

    {
        object* obj = make_object_int("int");
        object_int* t = get_object_true_type(obj);
        t->val = 81;
        push_object(obj);
    }

    {
        object* obj = make_object_float("float");
        object_float* f = get_object_true_type(obj);
        f->val = 3.1415;
        push_object(obj);
    }

    {
        object* obj = make_object_string("string");
        object_string* str = get_object_true_type(obj);
        string_push(str->val, "hello, world!");
        push_object(obj);
    }

    {
        object* obj = make_object_function("func");
        push_object(obj);
    }

    {
        object* obj = make_object_user_type("user_type");
        push_object(obj);
    }

    object* obj = find_object("bool");
    object_bool* b = get_object_true_type(obj);
    LOG_INFO("\t%d val %d\n", obj->type, b->val);

    obj = find_object("int");
    object_int* i = get_object_true_type(obj);
    LOG_INFO("\t%d val %d\n", obj->type, i->val);

    obj = find_object("float");
    object_float* f = get_object_true_type(obj);
    LOG_INFO("\t%d val %g\n", obj->type, f->val);

    obj = find_object("string");
    object_string* str = get_object_true_type(obj);
    LOG_INFO("\t%d val %s\n", obj->type, str->val);

    obj = find_object("func");
    LOG_INFO("\t%d\n", obj->type);

    obj = find_object("user_type");
    LOG_INFO("\t%d\n", obj->type);

    scopes_pop();

    shutdown_global_env();
    LOG_INFO("\tleak count = %d\n", check_memory_leak());
}

int main(void) {
    // test();
    // return 0;
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3.0+1";
    const char text[] = "var a=(2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5)+(.5-0.3-0.2)\n"
                        "var cat = 1-1.0-1--3*3\n"
                        "var dog = 1.0 + 101 % 3 / 2\n"
                        "dog += 1.3\n";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    vector(ast_node*) ins = parser_parse(&par);
    if (ins) {
        setup_global_env();
        scopes_push();
        for_vector(ins, i, 0) {
            command* cmd = ins[i]->gen_command(ins[i]);
            ASSERT(exec_command(cmd));
            cmd->destroy(cmd);
        }
        scopes_pop();
        shutdown_global_env();

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
