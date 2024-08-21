#include "command.h"
#include "container/memallocate.h"
#include "parser.h"
#include "container/string.h"
#include "core/assert.h"
#include "core/log.h"

static primitive_data command_binary_operation_cal(command* cmd) {
    switch (cmd->type) {
    case CommandTypeGetConstant: {
        command_get_constant* cst = get_command_true_type(cmd);
        if (cst->data.type[2] == PrimitiveDataTypeInt32) {
            LOG_TRACE("\texec cmd:%p CommandTypeGetConstant i32 %d\n", cmd, cst->data.int32);
        }
        else if (cst->data.type[2] == PrimitiveDataTypeFloat32) {
            LOG_TRACE("\texec CommandTypeGetConstant f32 %g\n", cst->data.float32);
        }
        return cst->data;
    }
    case CommandTypeNegateOperation: {
        command_negate_operation* no = get_command_true_type(cmd);
        primitive_data data = command_binary_operation_cal(no->data);
        LOG_TRACE("\texec cmd:%p CommandTypeNegateOperation\n", cmd);
        return primitive_data_negate(&data);
    }
    case CommandTypeBinaryOperation: {
        command_binary_operation* bo = get_command_true_type(cmd);
        return bo->cal(cmd);
    }
    default:
        ASSERT_MSG(0, "undefine command type");
        return (primitive_data){0};
    }
}

#define IMPL_COMMAND_BINARY_OPERATION(operation_name)\
    static primitive_data command_binary_operation_##operation_name(command* cmd) {\
        ASSERT(cmd->type == CommandTypeBinaryOperation);\
        LOG_TRACE("\texec cmd:%p CommandTypeBinaryOperation " #operation_name "\n", cmd);\
        command_binary_operation* bo = get_command_true_type(cmd);\
        primitive_data lhs = command_binary_operation_cal(bo->lhs);\
        if (lhs.type[2] < 0) return lhs;\
        primitive_data rhs = command_binary_operation_cal(bo->rhs);\
        if (rhs.type[2] < 0) return rhs;\
        return primitive_data_##operation_name(&lhs, &rhs);\
    }

IMPL_COMMAND_BINARY_OPERATION(add)
IMPL_COMMAND_BINARY_OPERATION(minus)
IMPL_COMMAND_BINARY_OPERATION(multiply)
IMPL_COMMAND_BINARY_OPERATION(divide)
IMPL_COMMAND_BINARY_OPERATION(modulus)

static void destroy_command_binary_operation(command* cmd) {
    ASSERT(cmd->type == CommandTypeBinaryOperation);
    command_binary_operation* bo = get_command_true_type(cmd);
    if (bo->lhs) {
        bo->lhs->destroy(bo->lhs);
    }
    if (bo->rhs) {
        bo->rhs->destroy(bo->rhs);
    }
    FREE(cmd);
}

static void destroy_command_negate_operation(command* cmd) {
    ASSERT(cmd->type == CommandTypeNegateOperation);
    command_negate_operation* no = get_command_true_type(cmd);
    ASSERT(no->data);
    no->data->destroy(no->data);
    FREE(cmd);
}

static void destory_command_get_constant(command* cmd) {
    ASSERT(cmd->type == CommandTypeGetConstant);
    FREE(cmd);
}

static void destroy_command_vardecl(command* cmd) {
    ASSERT(cmd->type == CommandTypeVarDecl);
    command_vardecl* vardecl = get_command_true_type(cmd);
    free_string(vardecl->variable_name);
    vardecl->expr->destroy(vardecl->expr);
    FREE(cmd);
}

static void* command_exec_vardecl(command* cmd) {
    ASSERT(cmd->type == CommandTypeVarDecl);
    command_vardecl* vardecl = get_command_true_type(cmd);
    LOG_TRACE("\texec cmd:%p CommandTypeVarDecl varname %s\n", cmd, vardecl->variable_name);

    // TODO: create variable from cmd->arg1
    primitive_data data = command_binary_operation_cal(vardecl->expr);
    if (data.type[2] < 0) {
        LOG_ERROR("\t%s on line %d\n", data.string, cmd->line_on_exec);
        return NULL;
    }
    LOG_INFO("\tvar %s = %g\n", vardecl->variable_name, data.float32);

    return cmd;
}

command* make_command(CommandType type, u64 type_size, i32 line, void*(*exec)(command*), void(*destroy)(command*)) {
    command* cmd = MALLOC(type_size + sizeof(command));
    cmd->exec = exec;
    cmd->destroy = destroy;
    cmd->type = type;
    cmd->line_on_exec = line;
    return cmd;
}

INLINE void* get_command_true_type(command* cmd) { return cmd + 1; }

command* make_command_get_constant(struct ast_node* node) {
    command* result = make_command(CommandTypeGetConstant, sizeof(command_get_constant), node->tok->line, NULL, destory_command_get_constant);
    command_get_constant* cst = get_command_true_type(result);
    cst->data = node->tok->val;
    if (cst->data.type[2] == PrimitiveDataTypeInt32) {
        LOG_TRACE("\tgen cmd:%p CommandTypeGetConstant i32 %d\n", result, cst->data.int32);
    }
    else if (cst->data.type[2] == PrimitiveDataTypeFloat32) {
        LOG_TRACE("\tgen cmd:%p CommandTypeGetConstant f32 %g\n", result, cst->data.float32);
    }
    return result;
}

#define IMPL_GEN_COMMAND_BINARY_OPERATION(operation_name)\
    command* make_command_##operation_name(ast_node* node) {\
        command* result = make_command(CommandTypeBinaryOperation, sizeof(command_binary_operation), node->tok->line, NULL, destroy_command_binary_operation);\
        command_binary_operation* bo = get_command_true_type(result);\
        bo->cal = command_binary_operation_##operation_name;\
        bo->lhs = node->lhs->gen_command(node->lhs);\
        bo->rhs = node->rhs->gen_command(node->rhs);\
        LOG_TRACE("\tgen cmd:%p CommandTypeBinaryOperation " #operation_name "\n", result);\
        return result;\
    }

IMPL_GEN_COMMAND_BINARY_OPERATION(add)
IMPL_GEN_COMMAND_BINARY_OPERATION(minus)
IMPL_GEN_COMMAND_BINARY_OPERATION(multiply)
IMPL_GEN_COMMAND_BINARY_OPERATION(divide)
IMPL_GEN_COMMAND_BINARY_OPERATION(modulus)

command* make_command_negate(ast_node* node) {
    command* result = make_command(CommandTypeNegateOperation, sizeof(command_negate_operation), node->tok->line, NULL, destroy_command_negate_operation);
    command_negate_operation* no = get_command_true_type(result);
    no->data = node->lhs->gen_command(node->lhs);
    LOG_TRACE("\tgen cmd:%p CommandTypeNegateOperation\n", result);
    return result;
}

command* make_command_vardecl(ast_node* node) {
    command* result = make_command(CommandTypeVarDecl, sizeof(command_vardecl), node->tok->line, command_exec_vardecl, destroy_command_vardecl);
    command_vardecl* vardecl = get_command_true_type(result);
    vardecl->variable_name = node->lhs->tok->val.string;
    vardecl->expr = node->rhs->gen_command(node->rhs);
    LOG_TRACE("\tgen cmd:%p CommandTypeVarDecl varname %s\n", result, vardecl->variable_name);
    return result;
}

