#include "command.h"
#include "container/memallocate.h"
#include "parser.h"
#include "container/string.h"
#include "core/assert.h"
#include "core/log.h"
#include "global.h"

static error_info command_binary_operation_cal(primitive_data* out, command* cmd) {
    switch (cmd->type) {
    case CommandTypeGetConstant: {
        command_get_constant* cst = get_command_true_type(cmd);
        if (cst->data.type[2] == PrimitiveDataTypeInt32) {
            LOG_TRACE("\texec cmd:%p CommandTypeGetConstant i32 %d\n", cmd, cst->data.int32);
        }
        else if (cst->data.type[2] == PrimitiveDataTypeFloat32) {
            LOG_TRACE("\texec CommandTypeGetConstant f32 %g\n", cst->data.float32);
        }
        *out = cst->data;
        break;
    }
    case CommandTypeNegateOperation: {
        command_negate_operation* no = get_command_true_type(cmd);
        primitive_data tmp;
        command_binary_operation_cal(&tmp, no->data);
        LOG_TRACE("\texec cmd:%p CommandTypeNegateOperation\n", cmd);
        return primitive_data_negate(out, &tmp);
    }
    case CommandTypeBinaryOperation: {
        command_binary_operation* bo = get_command_true_type(cmd);
        return bo->cal(out, cmd);
    }
    default:
        ASSERT_MSG(0, "undefine command type");
        return (error_info){ .msg = "undefine command type" };
    }
    return (error_info){ .msg = NULL };
}

#define IMPL_COMMAND_BINARY_OPERATION(operation_name)\
    static error_info command_binary_operation_##operation_name(primitive_data* out, command* cmd) {\
        ASSERT(cmd->type == CommandTypeBinaryOperation);\
        LOG_TRACE("\texec cmd:%p CommandTypeBinaryOperation " #operation_name "\n", cmd);\
        command_binary_operation* bo = get_command_true_type(cmd);\
        primitive_data lhs;\
        error_info ei = command_binary_operation_cal(&lhs, bo->lhs);\
        if (ei.msg) return ei;\
        primitive_data rhs;\
        ei = command_binary_operation_cal(&rhs, bo->rhs);\
        if (ei.msg) return ei;\
        return primitive_data_##operation_name(out, &lhs, &rhs);\
    }

IMPL_COMMAND_BINARY_OPERATION(add)
IMPL_COMMAND_BINARY_OPERATION(minus)
IMPL_COMMAND_BINARY_OPERATION(multiply)
IMPL_COMMAND_BINARY_OPERATION(divide)
IMPL_COMMAND_BINARY_OPERATION(modulus)

static object* access_object(command* cmd) {
    ASSERT(cmd->type == CommandTypeAccess);
    command_access* access = get_command_true_type(cmd);
    object* obj = find_object(access->name);
    if (!obj) {
        return make_object(ObjectErrorUndefine, access->name, 0, NULL);
    }

    for (command_access* ac = access; ac->access != NULL; ac = get_command_true_type(ac->access)) {
        // TODO: access member variable or funcation call
    }

    return obj;
}

static i32 command_assign_add(command* cmd) {
    ASSERT(cmd->type == CommandTypeAssignment);
    command_assign* ca = get_command_true_type(cmd);

    object* obj = access_object(ca->mem);
    if (obj->type == ObjectErrorUndefine) {
        LOG_ERROR("\tundefine variable '%s' on line %d\n", obj->name, ca->line_on_exec);
        FREE(obj);
        return 0;
    }

    if (obj->type != ObjectTypePrimitiveData) {
        LOG_ERROR("\tInvalid operands to binary expression on line %d\n", obj->name, ca->line_on_exec);
        return 0;
    }

    primitive_data data;
    error_info ei = command_binary_operation_cal(&data, ca->expr);
    if (ei.msg) {
        LOG_ERROR("\t%s on line %d\n", ei.msg, ca->line_on_exec);
        return 0;
    }

    object_primitive_data* o = get_object_true_type(obj);
    ei = primitive_data_add_assign(&o->val, &data);
    if (ei.msg) {
        LOG_ERROR("\t%s on line %d\n", ei.msg, ca->line_on_exec);
        return 0;
    }

    log_level_msg(LogLevelDebug, "\t%s += %g -> %g\n", obj->name, data.float32, o->val.float32);
    return 1;
}

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

static void destroy_command_assign(command* cmd) {
    ASSERT(cmd->type == CommandTypeAssignment);
    command_assign* ca = get_command_true_type(cmd);
    ca->mem->destroy(ca->mem);
    ca->expr->destroy(ca->expr);
    FREE(cmd);
}

static void destroy_command_negate_operation(command* cmd) {
    ASSERT(cmd->type == CommandTypeNegateOperation);
    command_negate_operation* no = get_command_true_type(cmd);
    ASSERT(no->data);
    no->data->destroy(no->data);
    FREE(cmd);
}

static void destroy_command_get_constant(command* cmd) {
    ASSERT(cmd->type == CommandTypeGetConstant);
    FREE(cmd);
}

static void destroy_command_access(command* cmd) {
    if (!cmd) {
        return;
    }
    ASSERT(cmd->type == CommandTypeAccess);
    command_access* access = get_command_true_type(cmd);
    free_string(access->name);
    cmd->destroy(access->access);
    FREE(cmd);
}

static void destroy_command_vardecl(command* cmd) {
    ASSERT(cmd->type == CommandTypeVarDecl);
    command_vardecl* vardecl = get_command_true_type(cmd);
    free_string(vardecl->variable_name);
    vardecl->expr->destroy(vardecl->expr);
    FREE(cmd);
}

static i32 command_exec_vardecl(command_vardecl* vardecl) {
    primitive_data data;
    error_info ei = command_binary_operation_cal(&data, vardecl->expr);
    if (ei.msg) {
        LOG_ERROR("\t%s on line %d\n", ei.msg, vardecl->line_on_exec);
        return 0;
    }
    LOG_DEBUG("\tvar %s = %g\n", vardecl->variable_name, data.float32);

    // NOTE: Check if the data is different from primitive in here.

    object* obj = make_object_primitive_data(vardecl->variable_name);
    object_primitive_data* o = get_object_true_type(obj);
    o->val = data;
    push_object(obj);

    return 1;
}

command* make_command(CommandType type, u64 type_size, void(*destroy)(command*)) {
    command* cmd = MALLOC(type_size + sizeof(command));
    cmd->destroy = destroy;
    cmd->type = type;
    return cmd;
}

INLINE void* get_command_true_type(command* cmd) { return cmd + 1; }

command* make_command_get_constant(struct ast_node* node) {
    command* result = make_command(CommandTypeGetConstant, sizeof(command_get_constant), destroy_command_get_constant);
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

command* make_command_access(struct ast_node* node) {
    command* result = make_command(CommandTypeAccess, sizeof(command_access), destroy_command_access);
    command_access* access = get_command_true_type(result);
    access->name = node->tok->val.string;
    access->access = NULL;
    ast_identifier* iden = get_ast_true_type(node);
    if (iden->next) {
        access->access = iden->next->gen_command(iden->next);
    }
    return result;
}

#define IMPL_GEN_COMMAND_BINARY_OPERATION(operation_name)\
    command* make_command_##operation_name(ast_node* node) {\
        command* result = make_command(CommandTypeBinaryOperation, sizeof(command_binary_operation), destroy_command_binary_operation);\
        command_binary_operation* bo = get_command_true_type(result);\
        ast_binary_expression* expr = get_ast_true_type(node);\
        bo->cal = command_binary_operation_##operation_name;\
        bo->lhs = expr->lhs->gen_command(expr->lhs);\
        bo->rhs = expr->rhs->gen_command(expr->rhs);\
        LOG_TRACE("\tgen cmd:%p CommandTypeBinaryOperation " #operation_name "\n", result);\
        return result;\
    }

IMPL_GEN_COMMAND_BINARY_OPERATION(add)
IMPL_GEN_COMMAND_BINARY_OPERATION(minus)
IMPL_GEN_COMMAND_BINARY_OPERATION(multiply)
IMPL_GEN_COMMAND_BINARY_OPERATION(divide)
IMPL_GEN_COMMAND_BINARY_OPERATION(modulus)

command* make_command_add_assign(struct ast_node* node) {
    command* result = make_command(CommandTypeAssignment, sizeof(command_assign), destroy_command_assign);
    command_assign* ca = get_command_true_type(result);
    ast_assignment* assignment = get_ast_true_type(node);
    ca->mem = assignment->variable_name->gen_command(assignment->variable_name);
    ca->expr = assignment->expr->gen_command(assignment->expr);
    ca->exec = command_assign_add;
    ca->line_on_exec = node->tok->line;
    return result;
}

command* make_command_minus_assign(struct ast_node* node);
command* make_command_multiply_assign(struct ast_node* node);
command* make_command_divide_assign(struct ast_node* node);
command* make_command_modulus_assign(struct ast_node* node);

command* make_command_negate(ast_node* node) {
    command* result = make_command(CommandTypeNegateOperation, sizeof(command_negate_operation), destroy_command_negate_operation);
    command_negate_operation* no = get_command_true_type(result);
    ast_negate* negate = get_ast_true_type(node);
    no->data = negate->term->gen_command(negate->term);
    LOG_TRACE("\tgen cmd:%p CommandTypeNegateOperation\n", result);
    return result;
}

command* make_command_vardecl(ast_node* node) {
    command* result = make_command(CommandTypeVarDecl, sizeof(command_vardecl), destroy_command_vardecl);
    command_vardecl* vardecl = get_command_true_type(result);
    ast_vardecl* vd = get_ast_true_type(node);
    vardecl->variable_name = vd->variable_name->tok->val.string;
    vardecl->expr = vd->expr->gen_command(vd->expr);
    LOG_TRACE("\tgen cmd:%p CommandTypeVarDecl varname %s\n", result, vardecl->variable_name);
    return result;
}

i32 exec_command(command* cmd) {
    switch (cmd->type) {
    case CommandTypeVarDecl:  {
        command_vardecl* vardecl = get_command_true_type(cmd);
        return command_exec_vardecl(vardecl);
    }
    case CommandTypeAssignment: {
        command_assign* ca = get_command_true_type(cmd);
        return ca->exec(cmd);
    }
    default: 
        ASSERT_MSG(0, "invalid command for execution");
        return 0;
    }
}

