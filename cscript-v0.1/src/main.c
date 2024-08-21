#include <string.h>
#include <assert.h>
#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "command.h"
#include "parser.h"
#include "core/assert.h"

#include "container/hashmap.h"
#include "object.h"

typedef vector(object*) scope;
typedef vector(scope) scopes;

#define make_scope() make_vector(object*)
#define make_scopes() make_vector(scope)

void free_scope(scope sc) {
    for_vector(sc, i, 0) {
        sc[i]->destroy(sc[i]);
    }
    free_vector(sc);
}

void scopes_push(scopes s) {
    scope sc = make_scope();
    vector_push(s, sc);
}

void scopes_push_obj(scopes s, object* obj) {
    vector_push(vector_back(s), obj);
}

void scopes_pop(scopes s) {
    free_scope(vector_back(s));
    vector_pop(s);
}

void free_scopes(scopes s) {
    for_vector(s, i, 0) {
        free_scope(s[i]);
        s[i] = NULL;
    }
    free_vector(s);
}

u32 hash_object(void* data, u32 size) {
    ASSERT(data);
    object* obj = data;
    return sdbm(obj->name) % size;
}

static hashmap map;

object* find_object(cstring name) {
    object obj = { .name = name };
    vector(void*) result = hashmap_access_vector(&map, &obj);
    for (i64 i = vector_size(result) - 1; i > -1; --i) {
        object* obj = result[i];
        if (strcmp(obj->name, name) == 0) {
            return obj;
        }
    }
    return NULL;
}

void remove_object(cstring name) {
    object obj = { .name = name };
    vector(void*) result = hashmap_access_vector(&map, &obj);
    for (i64 i = vector_size(result) - 1; i > -1; --i) {
        object* obj = result[i];
        if (strcmp(obj->name, name) == 0) {
            obj->destroy(obj);
            for (i64 j = i; j < vector_size(result) - 1; j++) {
                result[j] = result[j + 1];
            }
            break;
        }
    }
}

void test(void) {
    map = make_hashmap(1 << 10, hash_object);
    scopes s = make_scopes();
    scopes_push(s);

    {
        object* obj = make_object_bool("bool");
        object_bool* b = get_object_true_type(obj);
        b->val = 1;
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    {
        object* obj = make_object_int("int");
        object_int* t = get_object_true_type(obj);
        t->val = 81;
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    {
        object* obj = make_object_float("float");
        object_float* f = get_object_true_type(obj);
        f->val = 3.1415;
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    {
        object* obj = make_object_string("string");
        object_string* str = get_object_true_type(obj);
        string_push(str->val, "hello, world!");
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    {
        object* obj = make_object_function("func");
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    {
        object* obj = make_object_user_type("user_type");
        scopes_push_obj(s, obj);
        hashmap_add(&map, obj);
    }

    object* obj = find_object("bool");
    object_bool* b = get_object_true_type(obj);
    LOG_INFO("%d val %d\n", obj->type, b->val);

    obj = find_object("int");
    object_int* i = get_object_true_type(obj);
    LOG_INFO("%d val %d\n", obj->type, i->val);

    obj = find_object("float");
    object_float* f = get_object_true_type(obj);
    LOG_INFO("%d val %g\n", obj->type, f->val);

    obj = find_object("string");
    object_string* str = get_object_true_type(obj);
    LOG_INFO("%d val %s\n", obj->type, str->val);

    obj = find_object("func");
    LOG_INFO("%d\n", obj->type);

    obj = find_object("user_type");
    LOG_INFO("%d\n", obj->type);

    free_scopes(s);
    free_hashmap(&map);
    LOG_INFO("leak count = %d\n", check_memory_leak());
}

int main(void) {
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3*3.0+1";
    const char text[] = "var a=(2+4*(3/(.2*10))+3-1-1)*1.1+(0.5+.5)+(.5-0.3-0.2)\n"
                        "var cat = 1-1.0-1--3*3\n"
                        "var dog = 1.0 + 101 % 3 / 2\n";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    vector(ast_node*) ins = parser_parse(&par);
    if (ins) {
        for_vector(ins, i, 0) {
            command* cmd = ins[i]->gen_command(ins[i]);
            assert(cmd->exec(cmd) == cmd);
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
