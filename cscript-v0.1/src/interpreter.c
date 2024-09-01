#include "interpreter.h"
#include "container/memallocate.h"
#include "environment.h"
#include "parser.h"
#include "container/string.h"
#include "core/assert.h"
#include "core/log.h"
#include "tracing.h"
#include "ast_node.h"
#include <string.h>

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

static error_info initialize_object(interpreter* inter, command* cmd, const char* name);
static error_info access_object(interpreter* inter, command* cmd, object_carrier** obj);
static error_info access_funcall(interpreter* inter, command* cmd, object_carrier** obj);
static error_info access_identifier(interpreter* inter, command* cmd, object_carrier** obj);
static error_info exec_command_funcdef(interpreter* inter, command* cmd);

static error_info command_binary_operation_cal(interpreter* inter, command* cmd, primitive_data* out) {
    switch (cmd->type) {
    case CommandTypeReference: {
        object_carrier* carrier = NULL;
        error_info ei = access_object(inter, cmd, &carrier);
        if (ei.msg) {
            return ei;
        }
        ASSERT(carrier->obj != NULL);
        if (carrier->obj->type != ObjectTypePrimitiveData) {
            return (error_info){ .msg = "Invalid operands to binary expression" };
        }
        object_primitive_data* o = get_object_true_type(carrier->obj);
        *out = o->val;
        if (carrier->obj->name[0] == '.') {
            carrier->obj->destroy(carrier->obj, &inter->env);
            FREE(carrier);
        }
        break;
    }
    case CommandTypeGetConstant: {
        const command_get_constant* cst = get_command_true_type(cmd);
        *out = cst->data;
        break;
    }
    case CommandTypeNegateOperation: {
        const command_negate_operation* no = get_command_true_type(cmd);
        primitive_data tmp;
        command_binary_operation_cal(inter, no->data, &tmp);
        return primitive_data_negate(out, &tmp);
    }
    case CommandTypeBinaryOperation: {
        const command_binary_operation* bo = get_command_true_type(cmd);
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
        ASSERT(cmd->type == CommandTypeBinaryOperation);\
        START_PROFILING()\
        const command_binary_operation* bo = get_command_true_type(cmd);\
        primitive_data lhs;\
        error_info ei = command_binary_operation_cal(inter, bo->lhs, &lhs);\
        if (ei.msg) return ei;\
        primitive_data rhs;\
        ei = command_binary_operation_cal(inter, bo->rhs, &rhs);\
        if (ei.msg) return ei;\
        END_PROFILING(__func__)\
        return primitive_data_##operation_name(out, &lhs, &rhs);\
    }

IMPL_COMMAND_BINARY_OPERATION(add)
IMPL_COMMAND_BINARY_OPERATION(minus)
IMPL_COMMAND_BINARY_OPERATION(multiply)
IMPL_COMMAND_BINARY_OPERATION(divide)
IMPL_COMMAND_BINARY_OPERATION(modulus)

static error_info access_funcall(interpreter* inter, command* cmd, object_carrier** carrier) {
    ASSERT(cmd->type == CommandTypeFuncall);
    START_PROFILING();
    const command_funcall* funcall = get_command_true_type(cmd);
    const command_argument* args = get_command_true_type(funcall->args);
    LOG_DEBUG("\tcall %s ", funcall->name);
    for_vector(args->args, i, 0) {
        primitive_data data;
        error_info ei = command_binary_operation_cal(inter, args->args[i], &data);
        if (ei.msg) {
            return ei;
        }
        LOG_DEBUG_MSG("%g ", data.float32);
    }

    LOG_DEBUG_MSG("\n");
    object_carrier* func = env_find_object(&inter->env, funcall->name);
    if (!func) {
        return (error_info){ .msg = "undeclared function", .line = funcall->line_on_exec };
    }
    if (func->obj->type == ObjectTypeRef) {
        object_ref* ref = get_object_true_type(func->obj);
        func = env_find_object(&inter->env, ref->ref_name);
    }
    if (!func) {
        return (error_info){ .msg = "undeclared function", .line = funcall->line_on_exec };
    }
    if (func->obj->type != ObjectTypeFunctionDef) {
        return (error_info){ .msg = "called object type is not a function", .line = funcall->line_on_exec };
    }

    object_function_def* def = get_object_true_type(func->obj);
    if (vector_size(args->args) > vector_size(def->args)) {
        return (error_info){ .msg = "too many arguments", .line = funcall->line_on_exec };
    }
    if (vector_size(args->args) < vector_size(def->args)) {
        return (error_info){ .msg = "missing arguments", .line = funcall->line_on_exec };
    }

    env_push_scope(&inter->env);
    for_vector(def->args, i, 0) {
        error_info ei = initialize_object(inter, args->args[i], def->args[i]);
        if (ei.msg) {
            ei.line = funcall->line_on_exec;
            return ei;
        }
    }
    for_vector(def->body, i, 0) {
        if (def->body[i]->type == CommandTypeReturn) {
            const command_return* ret = get_command_true_type(def->body[i]);
            if (ret->expr) {
                error_info ei = initialize_object(inter, ret->expr, ".ret");
                if (ei.msg) {
                    env_pop_scope(&inter->env);
                    return ei;
                }
                vector_pop(vector_back(inter->env.global));
                *carrier = env_find_object(&inter->env, ".ret");
                vector(void*) result = hashmap_access_vector(&inter->env.map, *carrier);
                vector_pop(result);
                break;
            }
            env_push_object(&inter->env, make_object_carrier(make_object_none(".ret")));
            *carrier = env_find_object(&inter->env, ".ret");
            vector_pop(vector_back(inter->env.global));
            vector(void*) result = hashmap_access_vector(&inter->env.map, *carrier);
            vector_pop(result);
            break;
        }
        error_info ei = exec_command(inter, def->body[i]);
        if (ei.msg) {
            ei.line = funcall->line_on_exec;
            return ei;
        }
    }
    env_pop_scope(&inter->env);

    END_PROFILING(__func__);
    
    return (error_info){ .msg = NULL };
}

static error_info access_identifier(interpreter* inter, command* cmd, object_carrier** carrier) {
    START_PROFILING()
    ASSERT(cmd->type == CommandTypeReferenceIdentifier);
    const command_access_identifier* access = get_command_true_type(cmd);
    *carrier = env_find_object(&inter->env, access->name);
    if (!*carrier) {
        return (error_info){ .msg = "undeclared variable"};
    }
    END_PROFILING(__func__)
    return (error_info){ .msg = NULL };
}

static error_info access_object(interpreter* inter, command* cmd, object_carrier** carrier) {
    ASSERT(cmd->type == CommandTypeReference);
    START_PROFILING()
    const command_reference* access = get_command_true_type(cmd);
    error_info ei = access->reference(inter, access->id, carrier);
    if (ei.msg) {
        return ei;
    }

    if (access->next_ref) {
        ei = access_object(inter, access->next_ref, carrier);
    }

    for (const command_reference* ac = access; ac->next_ref != NULL; ac = get_command_true_type(ac->next_ref)) {
        // TODO: access member variable or funcation call
    }
    END_PROFILING(__func__)
    return (error_info){ .msg = NULL };
}

#define IMPL_COMMAND_ASSIGNMENT(assign_name)\
    static error_info command_assign_##assign_name(interpreter* inter, const command* cmd) {\
        START_PROFILING()\
        ASSERT(cmd->type == CommandTypeAssignment);\
        const command_assign* ca = get_command_true_type(cmd);\
        object_carrier* carrier = NULL;\
        error_info ei = access_object(inter, ca->mem, &carrier);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        if (carrier->obj->type != ObjectTypePrimitiveData)\
            return (error_info){ .msg = "Invalid operands to binary expression", .line = ca->line_on_exec };\
        primitive_data data;\
        ei = command_binary_operation_cal(inter, ca->expr, &data);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        object_primitive_data* o = get_object_true_type(carrier->obj);\
        ei = primitive_data_##assign_name##_assign(&o->val, &data);\
        if (ei.msg) {\
            ei.line = ca->line_on_exec;\
            return ei;\
        }\
        LOG_DEBUG("\t%d: %s %s by %g -> %g\n", ca->line_on_exec, carrier->obj->name, #assign_name, data.float32, o->val.float32);\
        END_PROFILING(__func__)\
        return (error_info){ .msg = NULL };\
    }

IMPL_COMMAND_ASSIGNMENT(add)
IMPL_COMMAND_ASSIGNMENT(minus)
IMPL_COMMAND_ASSIGNMENT(multiply)
IMPL_COMMAND_ASSIGNMENT(divide)
IMPL_COMMAND_ASSIGNMENT(modulus)

static error_info command_assignment(interpreter* inter, const command* cmd) {
    START_PROFILING()
    const command_assign* ca = get_command_true_type(cmd);
    object_carrier* carrier = NULL;
    error_info ei = access_object(inter, ca->mem, &carrier);
    if (ei.msg) {
        ei.line = ca->line_on_exec;
        return ei;
    }

    if (ca->expr->type == CommandTypeReference) {
        const command_reference* ref_cmd = get_command_true_type(ca->expr);
        object_carrier* rvalue = NULL;
        error_info ei = access_object(inter, ca->expr, &rvalue);
        if (ei.msg) {
            return ei;
        }
        if (ref_cmd->id->type == CommandTypeFuncall) {
            if (carrier->obj->type != ObjectTypeRef) {
                carrier->obj->destroy(carrier->obj, &inter->env);
                carrier->obj = make_object_ref(carrier->obj->name);
            }
            rvalue->obj->name = carrier->obj->name;
            // TODO: assign the result of function
            // env_push_object(&inter->env, rvalue);
            ASSERT(0);
            LOG_DEBUG("\tvar %s = .ret\n", carrier->obj->name);
            return (error_info){ .msg = NULL };
        }
        if (ref_cmd->id->type == CommandTypeReferenceIdentifier) {
            if (rvalue->obj->type == ObjectTypeRef) {
                if (carrier->obj->type != ObjectTypeRef) {
                    cstring name = carrier->obj->name;
                    carrier->obj->destroy(carrier->obj, &inter->env);
                    carrier->obj = make_object_ref(name);
                    object_ref* ref = get_object_true_type(carrier->obj);
                    ref->ref_name = "";
                }
                object_ref* ref_rvalue = get_object_true_type(rvalue->obj);
                object_ref* ref = get_object_true_type(carrier->obj);
                object_carrier* target = env_find_object(&inter->env, ref_rvalue->ref_name);
                if (strcmp(ref->ref_name, ref_rvalue->ref_name)) {
                    target->obj->ref_count++;
                }
                ref->ref_name = ref_rvalue->ref_name;
                LOG_DEBUG("\tvar %s -> func %s\n", carrier->obj->name, ref->ref_name);
                return (error_info){ .msg = NULL };
            }
            if (rvalue->obj->type != ObjectTypePrimitiveData) {
                if (carrier->obj->type == ObjectTypePrimitiveData) {
                    cstring name = carrier->obj->name;
                    carrier->obj->destroy(carrier->obj, &inter->env);
                    carrier->obj = make_object_ref(name);
                    object_ref* ref = get_object_true_type(carrier->obj);
                    ref->ref_name = "";
                }
                object_ref* ref = get_object_true_type(carrier->obj);
                if (strcmp(ref->ref_name, rvalue->obj->name)) {
                    rvalue->obj->ref_count++;
                }
                ref->ref_name = rvalue->obj->name;
                LOG_DEBUG("\tvar %s -> func %s\n", carrier->obj->name, ref->ref_name);
                return (error_info){ .msg = NULL };
            }
        }
        else {
            ASSERT_MSG(0, "undefine command reference type");
        }
    }

    primitive_data data;
    ei = command_binary_operation_cal(inter, ca->expr, &data);
    if (ei.msg) {
        return ei;
    }

    if (carrier->obj->type != ObjectTypePrimitiveData) {
        cstring name = carrier->obj->name;
        carrier->obj->destroy(carrier->obj, &inter->env);
        carrier->obj = make_object_primitive_data(name);
    }
    object_primitive_data* o = get_object_true_type(carrier->obj);
    o->val = data;

    LOG_DEBUG("\tvar %s = %g\n", carrier->obj->name, data.float32);
    END_PROFILING(__func__)
    return (error_info){ .msg = NULL };
}

static error_info initialize_object(interpreter* inter, command* cmd, const char* name) {
    // NOTE: Check if the data is different from primitive.
    if (cmd->type == CommandTypeReference) {
        const command_reference* ref_cmd = get_command_true_type(cmd);
        object_carrier* carrier = NULL;
        error_info ei = access_object(inter, cmd, &carrier);
        if (ei.msg) {
            return ei;
        }
        if (ref_cmd->id->type == CommandTypeFuncall) {
            carrier->obj->name = name;
            env_push_object(&inter->env, carrier);
            LOG_DEBUG("\tvar %s = .ret\n", name);
            return (error_info){ .msg = NULL };
        }
        if (ref_cmd->id->type == CommandTypeReferenceIdentifier) {
            if (carrier->obj->type == ObjectTypeRef) {
                object_ref* lvalue = get_object_true_type(carrier->obj);
                object* o = make_object_ref(name);
                object_ref* ref = get_object_true_type(o);
                ref->ref_name = lvalue->ref_name;
                object_carrier* target = env_find_object(&inter->env, ref->ref_name);
                target->obj->ref_count++;
                env_push_object(&inter->env, make_object_carrier(o));
                LOG_DEBUG("\tvar %s -> func %s\n", name, ref->ref_name);
                return (error_info){ .msg = NULL };
            }
            if (carrier->obj->type != ObjectTypePrimitiveData) {
                object* o = make_object_ref(name);
                object_ref* ref = get_object_true_type(o);
                ref->ref_name = carrier->obj->name;
                carrier->obj->ref_count++;
                env_push_object(&inter->env, make_object_carrier(o));
                LOG_DEBUG("\tvar %s -> func %s\n", name, ref->ref_name);
                return (error_info){ .msg = NULL };
            }
        }
        else {
            ASSERT_MSG(0, "undefine command reference type");
        }
    }

    primitive_data data;
    error_info ei = command_binary_operation_cal(inter, cmd, &data);
    if (ei.msg) {
        return ei;
    }

    object* obj = make_object_primitive_data(name);
    object_primitive_data* o = get_object_true_type(obj);
    o->val = data;

    env_push_object(&inter->env, make_object_carrier(obj));
    LOG_DEBUG("\tvar %s = %g\n", name, data.float32);
    return (error_info){ .msg = NULL };
}

static error_info command_exec_vardecl(interpreter* inter, const command_vardecl* vardecl) {
    START_PROFILING()

    object_carrier* carrier = env_find_object(&inter->env, vardecl->variable_name);
    if (carrier != NULL && carrier->obj->level == get_env_level(&inter->env)) {
        return (error_info){ .msg = "redeclare variable '%s'", .line = vardecl->line_on_exec };
    }

    error_info ei = initialize_object(inter, vardecl->expr, vardecl->variable_name);
    ei.line = vardecl->line_on_exec;
    END_PROFILING(__func__)
    return ei;
}

static error_info exec_command_funcdef(interpreter* inter, command* cmd) {
    START_PROFILING()
    ASSERT(cmd->type == CommandTypeFuncDef);
    const command_funcdef* def = get_command_true_type(cmd);
    LOG_DEBUG("\tfunc %s ", def->identifier);
    const command_funcparam* p = get_command_true_type(def->params);
    for_vector(p->params, i, 0) {
        LOG_DEBUG_MSG("%s ", p->params[i]);
    }

    LOG_DEBUG_MSG("\n");
    object* obj = make_object_function_def(def->identifier);
    object_function_def* def_obj = get_object_true_type(obj);
    def_obj->body = def->ins;
    def_obj->args = p->params;
    env_push_object(&inter->env, make_object_carrier(obj));
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

error_info exec_command(interpreter* inter, command* cmd) {
    switch (cmd->type) {
    case CommandTypeVarDecl:  {
        const command_vardecl* vardecl = get_command_true_type(cmd);
        return command_exec_vardecl(inter, vardecl);
    }
    case CommandTypeAssignment: {
        const command_assign* ca = get_command_true_type(cmd);
        return ca->exec(inter, cmd);
    }
    case CommandTypeReference: {
        object_carrier* carrier = NULL;
        error_info ei = access_object(inter, cmd, &carrier);
        carrier = env_find_object(&inter->env, ".ret");
        if (carrier) {
            carrier->obj->destroy(carrier->obj, &inter->env);
        }
        return ei;
    }
    case CommandTypeFuncDef: {
        return exec_command_funcdef(inter, cmd);
    }
    default: 
        ASSERT_MSG(0, "invalid command for execution");
        return (error_info){ .msg = "invalid command for execution" };
    }
}

error_info interpret_command(interpreter* inter) {
    command* cmd = inter->ins[inter->pointer++];
    return exec_command(inter, cmd);
}

static void destroy_command_binary_operation(command* cmd) {
    ASSERT(cmd->type == CommandTypeBinaryOperation);
    const command_binary_operation* bo = get_command_true_type(cmd);
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
    const command_assign* ca = get_command_true_type(cmd);
    ca->mem->destroy(ca->mem);
    ca->expr->destroy(ca->expr);
    FREE(cmd);
}

static void destroy_command_negate_operation(command* cmd) {
    ASSERT(cmd->type == CommandTypeNegateOperation);
    const command_negate_operation* no = get_command_true_type(cmd);
    ASSERT(no->data);
    no->data->destroy(no->data);
    FREE(cmd);
}

static void destroy_command_get_constant(command* cmd) {
    ASSERT(cmd->type == CommandTypeGetConstant);
    FREE(cmd);
}

static void destroy_command_reference(command* cmd) {
    ASSERT(cmd->type == CommandTypeReference);
    const command_reference* access = get_command_true_type(cmd);
    access->id->destroy(access->id);
    if (access->next_ref) {
        access->next_ref->destroy(access->next_ref);
    }
    FREE(cmd);
}

static void destroy_command_arguments(command* cmd) {
    ASSERT(cmd->type == CommandTypeArgument);
    const command_argument* argument = get_command_true_type(cmd);
    for_vector(argument->args, i, 0) {
        argument->args[i]->destroy(argument->args[i]);
    }
    free_vector(argument->args);
    FREE(cmd);
}

static void destroy_command_access_identifier(command* cmd) {
    ASSERT(cmd->type == CommandTypeReferenceIdentifier);
    const command_access_identifier* access = get_command_true_type(cmd);
    free_string(access->name);
    FREE(cmd);
}

static void destroy_command_funcparam(command* cmd) {
    ASSERT(cmd->type == CommandTypeFuncParam);
    const command_funcparam* param = get_command_true_type(cmd);
    for_vector(param->params, i, 0) {
        free_string(param->params[i]);
    }
    free_vector(param->params);
    FREE(cmd);
}

static void destroy_command_funcdef(command* cmd) {
    ASSERT(cmd->type == CommandTypeFuncDef);
    const command_funcdef* def = get_command_true_type(cmd);
    for_vector(def->ins, i, 0) {
        def->ins[i]->destroy(def->ins[i]);
    }
    free_vector(def->ins);
    if (def->params) {
        def->params->destroy(def->params);
    }
    free_string(def->identifier);
    FREE(cmd);
}

static void destroy_command_funcall(command* cmd) {
    ASSERT(cmd->type == CommandTypeFuncall);
    const command_funcall* funcall = get_command_true_type(cmd);
    funcall->args->destroy(funcall->args);
    if (funcall->name[0] != '.') {
        free_string(funcall->name);
    }
    FREE(cmd);
}

static void destroy_command_return(command* cmd) {
    ASSERT(cmd->type == CommandTypeReturn);
    const command_return* ret = get_command_true_type(cmd);
    if (ret->expr) {
        ret->expr->destroy(ret->expr);
    }
    FREE(cmd);
}

static void destroy_command_vardecl(command* cmd) {
    ASSERT(cmd->type == CommandTypeVarDecl);
    const command_vardecl* vardecl = get_command_true_type(cmd);
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

// INLINE void* get_command_true_type(const command* cmd) { return (command*)cmd + 1; }
INLINE const void* get_command_true_type(const command* cmd) { return cmd + 1; }

command* make_command_get_constant(struct ast_node* node) {
    command* result = make_command(CommandTypeGetConstant, sizeof(command_get_constant), destroy_command_get_constant);
    command_get_constant* cst = (command_get_constant*)(result + 1);
    cst->data = node->tok->val;
    return result;
}

command* make_command_argument(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeArgument, sizeof(command_argument), destroy_command_arguments);
    command_argument* argument = (command_argument*)(result + 1);
    argument->args = make_vector(command*);
    for (ast_node* n = node; n != NULL;) {
        ast_arg* arg = get_ast_true_type(n);
        if (arg->expr) {
            command* cmd = arg->expr->gen_command(arg->expr);
            vector_push(argument->args, cmd);
        }
        n = arg->next_arg;
    }
    END_PROFILING(__func__)
    return result;
}

command* make_command_funcparam(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeFuncParam, sizeof(command_funcparam), destroy_command_funcparam);
    command_funcparam* funcparam = (command_funcparam*)(result + 1);

    funcparam->params = make_vector(const char*);
    for (ast_node* n = node; n != NULL;) {
        vector_push(funcparam->params, n->tok->val.string);
        ast_funcparam* param = get_ast_true_type(n);
        n = param->next_param;
    }
    END_PROFILING(__func__)
    return result;
}

command* make_command_funcdef(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeFuncDef, sizeof(command_funcdef), destroy_command_funcdef);
    command_funcdef* funcdef = (command_funcdef*)(result + 1);
    ast_funcdef* def = get_ast_true_type(node);
    funcdef->identifier = node->tok->val.string;

    ast_funcparam* param = get_ast_true_type(def->param);
    funcdef->params = def->param->gen_command(param->next_param);

    funcdef->ins = make_vector(command*);
    for_vector(def->body, i, 0) {
        command* ins = def->body[i]->gen_command(def->body[i]);
        vector_push(funcdef->ins, ins);
    }
    END_PROFILING(__func__)
    return result;
}

command* make_command_funcall(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeFuncall, sizeof(command_funcall), destroy_command_funcall);
    command_funcall* funcall = (command_funcall*)(result + 1);
    funcall->name = node->tok->val.string;
    funcall->line_on_exec = node->tok->line;
    ast_funcall* f = get_ast_true_type(node);
    funcall->args = f->args->gen_command(f->args);
    END_PROFILING(__func__)
    return result;
}

command* make_command_return(struct ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeReturn, sizeof(command_return), destroy_command_return);
    command_return* ret = (command_return*)(result + 1);
    ast_return* ret_node = get_ast_true_type(node);
    ret->expr = NULL;
    if (ret_node->expr) {
        ret->expr = ret_node->expr->gen_command(ret_node->expr);
    }
    END_PROFILING(__func__)
    return result;
}

INLINE static command* make_command_reference(struct ast_node* node, error_info(*reference)(interpreter*, command*, object_carrier**)) {
    START_PROFILING()
    command* result = make_command(CommandTypeReference, sizeof(command_reference), destroy_command_reference);
    command_reference* access = (command_reference*)(result + 1);
    ast_reference* iden = get_ast_true_type(node);
    access->reference = reference;
    access->next_ref = NULL;
    access->id = iden->id->gen_command(iden->id);
    if (iden->next) {
        access->next_ref = iden->next->gen_command(iden->next);
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
    command* result = make_command(CommandTypeReferenceIdentifier, sizeof(command_access_identifier), destroy_command_access_identifier);
    command_access_identifier* iden = (command_access_identifier*)(result + 1);
    iden->name = node->tok->val.string;
    END_PROFILING(__func__)
    return result;
}

#define IMPL_GEN_COMMAND_BINARY_OPERATION(operation_name)\
    command* make_command_##operation_name(ast_node* node) {\
        START_PROFILING()\
        command* result = make_command(CommandTypeBinaryOperation, sizeof(command_binary_operation), destroy_command_binary_operation);\
        command_binary_operation* bo = (command_binary_operation*)(result + 1);\
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
        command_assign* ca = (command_assign*)(result + 1);\
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
    command_assign* ca = (command_assign*)(result + 1);
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
    command_negate_operation* no = (command_negate_operation*)(result + 1);
    ast_negate* negate = get_ast_true_type(node);
    no->data = negate->term->gen_command(negate->term);
    END_PROFILING(__func__)
    return result;
}

command* make_command_vardecl(ast_node* node) {
    START_PROFILING()
    command* result = make_command(CommandTypeVarDecl, sizeof(command_vardecl), destroy_command_vardecl);
    command_vardecl* vardecl = (command_vardecl*)(result + 1);
    ast_vardecl* vd = get_ast_true_type(node);
    vardecl->variable_name = vd->variable_name->tok->val.string;
    vardecl->expr = vd->expr->gen_command(vd->expr);
    vardecl->line_on_exec = node->tok->line;
    END_PROFILING(__func__)
    return result;
}

