#include "platform/platform.h"
#include "core/log.h"
#include "lexer.h"
#include "container/memallocate.h"

#include "parser.h"
#include "core/assert.h"

#include "environment.h"
#include "tracing.h"
#include "vm.h"
#include "bytecode.h"

__attribute__((destructor(101)))
static void check_leak(void) {
    LOG_DEBUG("\tleak count = %d\n", check_memory_leak());
}

void print_bytecode(vm* v) {
    for_vector(v->code, i, 0) {
        u8 code = v->code[i];
        switch (code) {
        case ByteCodePushConst: {
            LOG_DEBUG("\tpush %lld:%d\n", *((i64*)&v->code[i+2]), v->code[i+1]);
            i += 9;
        } break;
        case ByteCodeAdd: LOG_DEBUG("\tadd\n"); break;
        case ByteCodeSub: LOG_DEBUG("\tsub\n"); break;
        case ByteCodeMul: LOG_DEBUG("\tmul\n"); break;
        case ByteCodeDiv: LOG_DEBUG("\tdiv\n"); break;
        case ByteCodeMod: LOG_DEBUG("\tmod\n"); break;
        case ByteCodeNegate: LOG_DEBUG("\tmod\n"); break;
        case ByteCodeInitVar: LOG_DEBUG("\tinitvar\n"); break;
        case ByteCodePushRef: {
            LOG_DEBUG("\tpush %s\n", ((string)&v->code[i+1]));
            i+=8;
            break;
        }
        default: {
            LOG_ERROR("invalid bytecode %d\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return;
        }
        }
    }
    LOG_DEBUG("\n");
}

error_info run_bytecode(vm* v) {
    START_PROFILING();
    for_vector(v->code, i, 0) {
        u8 code = v->code[i];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = {
                .val = { v->code[i+2] },
                .type = v->code[i+1]
            };
            LOG_DEBUG("\tpush %d:%d\n", data.val.int64, data.type);
            vector_push(v->env.bp, data);
            i += 9;
        } break;
        case ByteCodeAdd: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_add(&data, v->env.bp + end - 2, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_popn(v->env.bp, 2);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tadd -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeSub: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_minus(&data, v->env.bp + end - 2, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_popn(v->env.bp, 2);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tsub -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeMul: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_multiply(&data, v->env.bp + end - 2, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_popn(v->env.bp, 2);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tmul -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeDiv: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_divide(&data, v->env.bp + end - 2, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_popn(v->env.bp, 2);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tdiv -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeMod: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_modulus(&data, v->env.bp + end - 2, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_popn(v->env.bp, 2);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tmod -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeNegate: {
            primitive_data data;
            u32 end = vector_size(v->env.bp);
            error_info ei = primitive_data_negate(&data, v->env.bp + end - 1);
            if (ei.msg) {
                return ei;
            }
            vector_pop(v->env.bp);
            vector_push(v->env.bp, data);
            LOG_DEBUG("\tneg -> %d:%d\n", data.val.int64, data.type);
        } break;
        case ByteCodeInitVar: {
            cstring name = vector_backn(v->env.bp, 0).val.string;
            primitive_data rhs = vector_backn(v->env.bp, 1);
            LOG_DEBUG("\tinitvar %s -> %d\n", name, rhs.val.int64);
            vector_popn(v->env.bp, 2);
        } break;
        case ByteCodePushRef: {
            primitive_data data = {
                .val.string = (string)&v->code[i+1],
                .type = PrimitiveDataTypeString,
            };
            vector_push(v->env.bp, data);
            i+=8;
            break;
        }
        default: {
            LOG_ERROR("invalid bytecode '%c'\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return (error_info){ .msg = "undefine bytecode" };
        }
        }
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

int main(void) {
    platform_state state;
    ASSERT(setup_platform(&state));

    lexer lex = {NULL, -1, 1, 1, 0};
    lexer_load_file_text(&lex, "test.cscript");

    parser par;
    init_parser(&par, generate_tokens(&lex));

    vector(ast_node*) ast = parser_parse(&par);
    for_vector(par.errors, i, 0) {
        LOG_ERROR("\t%d:%d %s\n", par.errors[i].line, par.errors[i].position, par.errors[i].msg);
    }

    if (ast) {
        vm v;
        init_vm(&v);
        for_vector(ast, i, 0) {
            ast[i]->gen_bytecode(ast[i], &v);
        }

        START_PROFILING();
        print_bytecode(&v);
        END_PROFILING("debug log");

        error_info ei = run_bytecode(&v);
        if (ei.msg) {
            LOG_ERROR("\t%s\n", ei.msg);
        }

        for_vector(par.tokens, i, 0) {
            if (par.tokens[i].type == TokenTypeIdentifier) {
                free_string(par.tokens[i].data.val.string);
            }
        }
        free_ast(ast);
        free_parser(&par);
        free_vm(&v);
    }
    else {
        free_parser(&par);
    }

    free_vector(lex.ctx);
    shutdown_platform(&state);

    return 0;
}

