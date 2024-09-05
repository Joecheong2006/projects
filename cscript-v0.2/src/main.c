#include "object.h"
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
        case ByteCodePushName: {
            LOG_DEBUG("\tpush %s\n", ((string)&v->code[i+1]));
            i+=8;
            break;
        }
        case ByteCodeRefIden: {
            LOG_DEBUG("\tref %s\n", ((string)&v->code[i+1]));
            i+=8;
            break;
        }
        default: {
            LOG_ERROR("\tinvalid bytecode %d\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return;
        }
        }
    }
    LOG_DEBUG("\n");
}
#include "timer.h"

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

        timer t;
        start_timer(&t);
        error_info ei = vm_run(&v);
        end_timer(&t);
        LOG_TRACE("\trun for %gs\n", t.dur);

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

