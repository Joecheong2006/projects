#include "platform/platform.h"
#include "core/log.h"
#include "core/memory.h"
#include "lexer.h"
#include "lexer.h"
#include "timer.h"

#include "parser.h"
#include "core/assert.h"

#include "primitive_data.h"
#include "tracing.h"
#include "vm.h"
#include "bytecode.h"
#include <string.h>

__attribute__((destructor(101)))
static void check_leak(void) {
    LOG_DEBUG("\tleak count = %d\n", get_allocation_count());
}

void print_bytecode(vm* v) {
    for_vector(v->code, i, 0) {
        u8 code = v->code[i];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = { .type = v->code[i+1] };
            memcpy(&data.val, &v->code[i+2], primitive_size_map[data.type]);
            LOG_DEBUG("\tpush\t\t");
            print_primitive_data(&data);
            i += primitive_size_map[data.type] + 1;
        } break;
        case ByteCodeAdd: LOG_DEBUG("\tadd\n"); break;
        case ByteCodeSub: LOG_DEBUG("\tsub\n"); break;
        case ByteCodeMul: LOG_DEBUG("\tmul\n"); break;
        case ByteCodeDiv: LOG_DEBUG("\tdiv\n"); break;
        case ByteCodeMod: LOG_DEBUG("\tmod\n"); break;
        case ByteCodeNegate: LOG_DEBUG("\tmod\n"); break;
        case ByteCodeInitVar: LOG_DEBUG("\tinitvar\n"); break;
        case ByteCodeAssign: LOG_DEBUG("\tassign\n"); break;
        case ByteCodeAddAssign: LOG_DEBUG("\tadd_assign\n"); break;
        case ByteCodeSubAssign: LOG_DEBUG("\tsub_assign\n"); break;
        case ByteCodeMulAssign: LOG_DEBUG("\tmul_assign\n"); break;
        case ByteCodeDivAssign: LOG_DEBUG("\tdiv_assign\n"); break;
        case ByteCodeModAssign: LOG_DEBUG("\tmod_assign\n"); break;
        case ByteCodePop: LOG_DEBUG("\tpop\n"); break;
        case ByteCodePushName: { LOG_DEBUG("\tpush\t%s\n", *(cstring*)&v->code[i+1]); i+=8; break; }
        case ByteCodeRefIden: { LOG_DEBUG("\tref %s\n", *(cstring*)&v->code[i+1]); i+=8; break; }
        case ByteCodeAccessIden: { LOG_DEBUG("\taccess %s\n", *(cstring*)&v->code[i+1]); i+=8; break; }
        case ByteCodeFuncDef: { LOG_DEBUG("\tfuncdef\n"); break; }
        case ByteCodeFuncEnd: { LOG_DEBUG("\tfuncend\n"); break; }
        case ByteCodeFuncall: { LOG_DEBUG("\tfuncall\n"); break; }
        case ByteCodeReturn: { LOG_DEBUG("\tret\n"); break; }
        case ByteCodeReturnNone: { LOG_DEBUG("\tret_none\n"); break; }
        default: {
            LOG_ERROR("\tinvalid bytecode %d\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return;
        }
        }
    }
    LOG_DEBUG("\n");
}

int main(void) {
    platform_state state;
    ASSERT(setup_platform(&state));

    lexer lex = {NULL, -1, 1, 1, 0};
    lexer_load_file_text(&lex, "test.script");

    parser par;
    init_parser(&par, generate_tokens(&lex));

    vector(ast_node*) ast = parser_parse(&par);
    for_vector(par.errors, i, 0) {
        LOG_ERROR("\t%d:%d %s\n", par.errors[i].line, par.errors[i].position, par.errors[i].msg);
    }

    if (ast) {
        vm v;
        init_vm(&v);
        vm_gen_bytecode(&v, ast);

        START_PROFILING();
        print_bytecode(&v);
        END_PROFILING("debug log");

        error_info ei;
        {
            START_PROFILING();
            ei = vm_run(&v);
            END_PROFILING("vm run");
        }
        LOG_DEBUG("\tstack %d\n", vector_size(v.env.bp));

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

