#include "platform/platform.h"
#include "core/log.h"
#include "core/memory.h"
#include "lexer.h"
#include "lexer.h"

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

void locating_position(vm* v) {
    vector(u32) position = make_vector(u32);
    for_vector(v->code, ip, 0) {
        u8 code = v->code[ip];
        switch (code) {
        case ByteCodePushConst: {
            primitive_data data = { .type = v->code[ip+1] };
            memcpy(&data.val, &v->code[ip+2], primitive_size_map[data.type]);
            ip += primitive_size_map[data.type] + 1;
            break;
        }
        case ByteCodePushNull:
        case ByteCodePushTrue:
        case ByteCodePushFalse:
        case ByteCodeAdd:
        case ByteCodeSub:
        case ByteCodeMul:
        case ByteCodeDiv:
        case ByteCodeMod:
        case ByteCodeNegate:
        case ByteCodeEqual:
        case ByteCodeNotEqual:
        case ByteCodeGreaterThan:
        case ByteCodeLessThan:
        case ByteCodeGreaterThanEqual:
        case ByteCodeLessThanEqual:
        case ByteCodeInitVar:
        case ByteCodeAssign:
        case ByteCodeAddAssign:
        case ByteCodeSubAssign:
        case ByteCodeMulAssign:
        case ByteCodeDivAssign:
        case ByteCodeModAssign:
        case ByteCodePop: break;
        case ByteCodePushName:
        case ByteCodeRefIden:
        case ByteCodeAccessIden: ip+=8; break;
        case ByteCodeFuncDef: {
            vector_push(position, ip);
            ip+=4;
            break;
        }
        case ByteCodeFuncEnd: {
            u32* pos = (u32*)(v->code + vector_back(position) + 1);
            *pos = ip;
            vector_pop(position);
            break;
        }
        case ByteCodeFuncall: break;
        case ByteCodeReturn: break;
        case ByteCodeReturnNone: break;
        default: {
            LOG_ERROR("\tinvalid bytecode %d\n", code);
            ASSERT_MSG(0, "invalid bytecode");
            return;
        }
        }
    }
    LOG_DEBUG("\n");
    free_vector(position);
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
        locating_position(&v);
        END_PROFILING("locating position");

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

