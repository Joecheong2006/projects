#include "interpreter.h"
#include "container/memallocate.h"
#include "parser.h"
#include "container/string.h"
#include "core/assert.h"
#include "core/log.h"
#include "tracing.h"
#include "ast_node.h"

static error_info access_object(interpreter* inter, command* cmd, object** obj);
static error_info command_binary_operation_cal(interpreter* inter, command* cmd, primitive_data* out);

static error_info command_binary_operation_add(interpreter* inter, command* cmd, primitive_data* out);
static error_info command_binary_operation_minus(interpreter* inter, command* cmd, primitive_data* out);
static error_info command_binary_operation_multiply(interpreter* inter, command* cmd, primitive_data* out);
static error_info command_binary_operation_divide(interpreter* inter, command* cmd, primitive_data* out);
static error_info command_binary_operation_modulus(interpreter* inter, command* cmd, primitive_data* out);

static error_info command_assignment(interpreter* inter, const command* cmd);
static error_info command_assign_add(interpreter* inter, const command* cmd);
static error_info command_assign_minus(interpreter* inter, const command* cmd);
static error_info command_assign_multiply(interpreter* inter, const command* cmd);
static error_info command_assign_divide(interpreter* inter, const command* cmd);
static error_info command_assign_modulus(interpreter* inter, const command* cmd);

static error_info access_funcall(interpreter* inter, command* cmd, object** obj);
static error_info access_identifier(interpreter* inter, command* cmd, object** obj);

static error_info command_binary_operation_cal(interpreter* inter, command* cmd, primitive_data* out) {
    switch (cmd->type) {
    case CommandTypeAccess: {
        object* obj = NULL;
        error_info ei = access_object(inter, cmd, &obj);
        if (ei.msg) {
            return ei;
        }
        else if (obj->type != ObjectTypePrimitiveData) {
            return (error_info){ .msg = "Invalid operands to binary expression" };
        }
        object_primitive_data* o = get_object_true_type(obj);
        *out = o->val;
        break;
    }
    case CommandTypeGetConstant: {
        START_PROFILING()
        command_get_constant* cst = get_command_true_type(cmd);
        *out = cst->data;
        END_PROFILING("CommandTypeGetConstant")
        break;
    }
    case CommandTypeNegateOperation: {
        command_negate_operation* no = get_command_true_type(cmd);
        primitive_data tmp;
        command_binary_operation_cal(inter, no->data, &tmp);
        return primitive_data_negate(out, &tmp);
    }
    case CommandTypeBinaryOperation: {
        command_binary_operation* bo = get_command_true_type(cmd);
        return bo->cal(inter, cmd, out);
    }
    default:
        ASSERT_MSG(0, "undefine command type");
        return (error_info){ .msg = "undefine command type" };
    }
    return (error_info){ .msg = NULL };
}

#define IMPL_COMMAND_BINARY_OPERATION(operation_name)\
    static error_info command_binary_operation_##operation_name(interpreter* inter, command* cmd, primitive_data* out) {\
        START_PROFILING()\
        ASSERT(cmd->type == CommandTypeBinaryOperation);\
        command_binary_operation* bo = get_command_true_type(cmd);\
        END_PROFILING(__func__)\
        primitive_data lhs;\
        error_info ei = command_binary_operation_cal(inter, bo->lhs, &lhs);\
        if (ei.msg) return ei;\
        primitive_data rhs;\
        ei = command_binary_operation_cal(inter, bo->rhs, &rhs);\
        if (ei.msg) return ei;\
        return primitive_data_##operation_name(out, &lhs, &rhs);\
    }

IMPL_COMMAND_BINARY_OPERATION(add)
IMPL_COMMAND_BINARY_OPERATION(minus)
IMPL_COMMAND_BINARY_OPERATION(multiply)
IMPL_COMMAND_BINARY_OPERATION(divide)
IMPL_COMMAND_BINARY_OPERATION(modulus)

static error_info access_funcall(interpreter* inter, command* cmd, object** obj) {
    ASSERT(cmd->type == CommandTypeFuncall);
    command_funcall* funcall = get_command_true_type(cmd);
    (void)inter, (void)funcall;
    *obj = NULL;
    return (error_info){ .msg = NULL };
}

static error_info access_identifier(interpreter* inter, command* cmd, object** obj) {
    ASSERT(cmd->type == CommandTypeAccessIdentifier);
    command_access_identifier* access = get_command_true_type(cmd);
    *obj = env_find_object(&inter->env, access->name);
    return (error_info){ .msg = NULL };
}

static error_info access_object(interpreter* inter, command* cmd, object** obj) {
    ASSERT(cmd->type == CommandTypeAccess);
    START_PROFILING()
    command_reference* access = get_command_true_type(cmd);
    error_info ei = access->reference(inter, access->id, obj);
    if (ei.msg) {
        return ei;
    }

    for (command_reference* ac = access; ac->next_access != NULL; ac = get_command_true_type(ac->next_access)) {
        // TODO: access member variable or funcation call
    }
    END_PROFILING(__func__)
    return (error_info){ .msg = NULL };
}

#define IMPL_COMMAND_ASSIGNMENT(assign_name)\
    static error_info command_assign_##assign_name(interpreter* inter, const command* cmd) {\
        START_PROFILING()\
        ASSERT(cmd->type == CommandTypeAssignment);\
        command_assign* ca = get_command_true_type(cmd);\
        object* obj = NULL;\
        error_info ei = access_object(inter, ca->mem, &obj);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        if (obj->type != ObjectTypePrimitiveData)\
            return (error_info){ .msg = "Invalid operands to binary expression", .line = ca->line_on_exec };\
        primitive_data data;\
        ei = command_binary_operation_cal(inter, ca->expr, &data);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        object_primitive_data* o = get_object_true_type(obj);\
        ei = primitive_data_##assign_name##_assign(&o->val, &data);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        LOG_DEBUG("\t%d: %s %s by %g -> %g\n", ca->line_on_exec, obj->name, #assign_name, data.float32, o->val.float32);\
        END_PROFILING(__func__)\
        return (error_info){ .msg = NULL };\
    }

IMPL_COMMAND_ASSIGNMENT(add)
IMPL_COMMAND_ASSIGNMENT(minus)
IMPL_COMMAND_ASSIGNMENT(multiply)
IMPL_COMMAND_ASSIGNMENT(divide)
IMPL_COMMAND_ASSIGNMENT(modulus)

static error_info command_assignment(interpreter* inter, const command* cmd) {
    START_PROFILING();
    command_assign* ca = get_command_true_type(cmd);
    object* obj = NULL;
    error_info ei = access_object(inter, ca->mem, &obj);
    if (ei.msg) {
        ei.line = ca->line_on_exec;
        return ei;
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

static error_info command_exec_vardecl(interpreter* inter, command_vardecl* vardecl) {
    START_PROFILING()
    object* obj = NULL;
    // NOTE: Check if the data is different from primitive.

    if (env_find_object(&inter->env, vardecl->variable_name) != NULL) {
        // LOG_ERROR("\tredeclare variable '%s' on line %d\n", vardecl->variable_name, vardecl->line_on_exec);
        return (error_info){ .msg = "redeclare variable '%s'", .line = vardecl->line_on_exec };
    }

    primitive_data data;
    error_info ei = command_binary_operation_cal(inter, vardecl->expr, &data);
    if (ei.msg) {
        ei.line = vardecl->line_on_exec;
        return ei;
    }

    obj = make_object_primitive_data(vardecl->variable_name);
    object_primitive_data* o = get_object_true_type(obj);
    o->val = data;

    env_push_object(&inter->env, obj);
    LOG_DEBUG("\t%d: var %s = %g\n", vardecl->line_on_exec, vardecl->variable_name, data.float32);
    END_PROFILING(__func__)
    return (error_info){ .msg = NULL };
}

INLINE void init_interpreter(interpreter* inter, vector(command*) ins) {
    START_PROFILING();
    inter->ins = ins;
    inter->pointer = 0;
    init_environment(&inter->env);
    END_PROFILING(__func__);
}

INLINE void free_interpreter(interpreter* inter) {
    START_PROFILING();
    for_vector(inter->ins, i, 0) {
        inter->ins[i]->destroy(inter->ins[i]);
    }
    free_vector(inter->ins);
    free_environment(&inter->env);
    inter->pointer = -1;
    END_PROFILING(__func__);
}

error_info interpret_command(interpreter* inter) {
    command* cmd = inter->ins[inter->pointer++];
    switch (cmd->type) {
    case CommandTypeVarDecl:  {
        command_vardecl* vardecl = get_command_true_type(cmd);
        return command_exec_vardecl(inter, vardecl);
    }
    case CommandTypeAssignment: {
        command_assign* ca = get_command_true_type(cmd);
        return ca->exec(inter, cmd);
    }
    default: 
        ASSERT_MSG(0, "invalid command for execution");
        return (error_info){ .msg = "invalid command for execution" };
    }
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
    command_reference* access = get_command_true_type(cmd);
    access->id->destroy(access->id);
    if (access->next_access) {
        access->next_access->destroy(access->next_access);
    }
    FREE(cmd);
}

static void destroy_command_arguments(command* cmd) {
    ASSERT(cmd->type == CommandTypeArgument);
    command_argument* argument = get_command_true_type(cmd);
    if (argument->expr) {
        argument->expr->destroy(argument->expr);
    }
    if (argument->next_arg) {
        argument->next_arg->destroy(argument->next_arg);
    }
    FREE(cmd);
}

static void destroy_command_access_identifier(command* cmd) {
    ASSERT(cmd->type == CommandTypeAccessIdentifier);
    command_access_identifier* access = get_command_true_type(cmd);
    free_string(access->name);
    FREE(cmd);
}

static void destroy_command_funcall(command* cmd) {
    ASSERT(cmd->type == CommandTypeFuncall);
    command_funcall* funcall = get_command_true_type(cmd);
    funcall->args->destroy(funcall->args);
    free_string(funcall->name);
    FREE(cmd);
}

static void destroy_command_vardecl(command* cmd) {
    ASSERT(cmd->type == CommandTypeVarDecl);
    command_vardecl* vardecl = get_command_true_type(cmd);
    free_string(vardecl->variable_name);
    vardecl->expr->destroy(vardecl->expr);
    FREE(cmd);
}

INLINE command* make_command(CommandType type, u64 type_size, void(*destroy)(command*)) {
    command* cmd = MALLOC(type_size + sizeof(command));
    cmd->destroy = destroy;
    cmd->type = type;
    return cmd;
}

INLINE void* get_command_true_type(const command* cmd) { return (command*)cmd + 1; }

command* make_command_get_constant(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeGetConstant, sizeof(command_get_constant), destroy_command_get_constant);
    command_get_constant* cst = get_command_true_type(result);
    cst->data = node->tok->val;
    END_PROFILING(__func__)
    return result;
}

command* make_command_argument(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeArgument, sizeof(command_argument), destroy_command_arguments);
    command_argument* argument = get_command_true_type(result);
    ast_arg* arg = get_ast_true_type(node);
    argument->expr = arg->expr->gen_command(arg->expr);
    if (arg->next_param) {
        argument->next_arg = arg->next_param->gen_command(arg->next_param);
    }
    END_PROFILING(__func__)
    return result;
}

command* make_command_funcall(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeFuncall, sizeof(command_funcall), destroy_command_funcall);
    command_funcall* funcall = get_command_true_type(result);
    ast_funcall* f = get_ast_true_type(node);
    funcall->name = node->tok->val.string;
    funcall->args = f->args->gen_command(f->args);
    END_PROFILING(__func__)
    return result;
}

INLINE static command* make_command_reference(struct ast_node* node, error_info(*reference)(interpreter*, command*, object**)) {
    START_PROFILING()
    command* result = make_command(CommandTypeAccess, sizeof(command_reference), destroy_command_access);
    command_reference* access = get_command_true_type(result);
    ast_reference* iden = get_ast_true_type(node);
    access->next_access = NULL;
    access->reference = reference;
    access->id = iden->id->gen_command(iden->id);
    if (iden->next) {
        access->next_access = iden->next->gen_command(iden->next);
    }
    END_PROFILING(__func__)
    return result;
}

command* make_command_reference_identifier(struct ast_node* node) {
    return make_command_reference(node, access_identifier);
}

command* make_command_reference_funcall(struct ast_node* node) {
    return make_command_reference(node, access_funcall);
}

command* make_command_access_identifier(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeAccessIdentifier, sizeof(command_access_identifier), destroy_command_access_identifier);
    command_access_identifier* iden = get_command_true_type(result);
    iden->name = node->tok->val.string;
    END_PROFILING(__func__)
    return result;
}

#define IMPL_GEN_COMMAND_BINARY_OPERATION(operation_name)\
    command* make_command_##operation_name(ast_node* node) {\
        START_PROFILING()\
        command* result = make_command(CommandTypeBinaryOperation, sizeof(command_binary_operation), destroy_command_binary_operation);\
        command_binary_operation* bo = get_command_true_type(result);\
        ast_binary_expression* expr = get_ast_true_type(node);\
        bo->cal = command_binary_operation_##operation_name;\
        END_PROFILING(__func__)\
        bo->lhs = expr->lhs->gen_command(expr->lhs);\
        bo->rhs = expr->rhs->gen_command(expr->rhs);\
        return result;\
    }

IMPL_GEN_COMMAND_BINARY_OPERATION(add)
IMPL_GEN_COMMAND_BINARY_OPERATION(minus)
IMPL_GEN_COMMAND_BINARY_OPERATION(multiply)
IMPL_GEN_COMMAND_BINARY_OPERATION(divide)
IMPL_GEN_COMMAND_BINARY_OPERATION(modulus)

#define IMPL_GEN_COMMAND_ASSIGNMENT(assign_name)\
    command* make_command_##assign_name##_assign(struct ast_node* node) {\
        START_PROFILING()\
        command* result = make_command(CommandTypeAssignment, sizeof(command_assign), destroy_command_assign);\
        command_assign* ca = get_command_true_type(result);\
        ast_assignment* assignment = get_ast_true_type(node);\
        ca->exec = command_assign_##assign_name;\
        ca->line_on_exec = node->tok->line;\
        ca->mem = assignment->variable_name->gen_command(assignment->variable_name);\
        ca->expr = assignment->expr->gen_command(assignment->expr);\
        END_PROFILING(__func__)\
        return result;\
    }

IMPL_GEN_COMMAND_ASSIGNMENT(add)
IMPL_GEN_COMMAND_ASSIGNMENT(minus)
IMPL_GEN_COMMAND_ASSIGNMENT(multiply)
IMPL_GEN_COMMAND_ASSIGNMENT(divide)
IMPL_GEN_COMMAND_ASSIGNMENT(modulus)

command* make_command_assignment(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeAssignment, sizeof(command_assign), destroy_command_assign);
    command_assign* ca = get_command_true_type(result);
    ast_assignment* assignment = get_ast_true_type(node);
    ca->exec = command_assignment;
    ca->line_on_exec = node->tok->line;
    ca->mem = assignment->variable_name->gen_command(assignment->variable_name);
    ca->expr = assignment->expr->gen_command(assignment->expr);
    END_PROFILING(__func__)
    return result;
}

command* make_command_negate(ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeNegateOperation, sizeof(command_negate_operation), destroy_command_negate_operation);
    command_negate_operation* no = get_command_true_type(result);
    ast_negate* negate = get_ast_true_type(node);
    no->data = negate->term->gen_command(negate->term);
    END_PROFILING(__func__)
    return result;
}

command* make_command_vardecl(ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeVarDecl, sizeof(command_vardecl), destroy_command_vardecl);
    command_vardecl* vardecl = get_command_true_type(result);
    ast_vardecl* vd = get_ast_true_type(node);
    vardecl->variable_name = vd->variable_name->tok->val.string;
    vardecl->expr = vd->expr->gen_command(vd->expr);
    vardecl->line_on_exec = node->tok->line;
    END_PROFILING(__func__)
    return result;
}

