#include "bytecode.h"
#include "ast_node.h"
#include "core/assert.h"
#include "tracing.h"
#include "lexer.h"
#include "vm.h"
#include <string.h>

static void gen_ptr(const void* ptr, vm* v) {
    START_PROFILING();
    u32 end = vector_size(v->code);
    vector_resize(v->code, end + 8);
    memcpy(v->code + end, ptr, 8);
    END_PROFILING(__func__);
}

static void gen_const(void* val, u8 type, u8 size, vm* v) {
    START_PROFILING();
    vector_push(v->code, type);
    u32 end = vector_size(v->code);
    vector_resize(v->code, end + size);
    memcpy(v->code + end, val, size);
    END_PROFILING(__func__);
}

void gen_bytecode_bracket(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeExprBracket);
    ast_expression_bracket* bracket = get_ast_true_type(node);
    bracket->expr->gen_bytecode(bracket->expr, v);
}

#define IMPL_ARITHMETIC(arith_name, flag)\
    void gen_bytecode_##arith_name(ast_node* node, vm* v) {\
        ASSERT(node->type == AstNodeTypeExpr##flag);\
        ast_binary_expression* expr = get_ast_true_type(node);\
        expr->lhs->gen_bytecode(expr->lhs, v);\
        expr->rhs->gen_bytecode(expr->rhs, v);\
        START_PROFILING();\
        u8 code = ByteCode##flag;\
        vector_push(v->code, code);\
        END_PROFILING(__func__);\
    }

IMPL_ARITHMETIC(add, Add)
IMPL_ARITHMETIC(sub, Sub)
IMPL_ARITHMETIC(mul, Mul)
IMPL_ARITHMETIC(div, Div)
IMPL_ARITHMETIC(mod, Mod)

void gen_bytecode_negate(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeNegate);
    ast_negate* neg = get_ast_true_type(node);
    neg->term->gen_bytecode(neg->term, v);
    START_PROFILING();
    u8 code = ByteCodeNegate;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_assign(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeAssignment);
    ast_assignment* assignment = get_ast_true_type(node);
    assignment->expr->gen_bytecode(assignment->expr, v);
    assignment->name->gen_bytecode(assignment->name, v);
    START_PROFILING();
    u8 code = ByteCodeAssign;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}


#define IMPL_ASSIGNMENT(func_name, flag)\
    void gen_bytecode_##func_name##_assign(struct ast_node* node, struct vm* v) {\
        ASSERT(node->type == AstNodeType##flag);\
        ast_assignment* assignment = get_ast_true_type(node);\
        assignment->expr->gen_bytecode(assignment->expr, v);\
        assignment->name->gen_bytecode(assignment->name, v);\
        START_PROFILING();\
        u8 code = ByteCode##flag;\
        vector_push(v->code, code);\
        END_PROFILING(__func__);\
    }

IMPL_ASSIGNMENT(add, AddAssign)
IMPL_ASSIGNMENT(sub, SubAssign)
IMPL_ASSIGNMENT(mul, MulAssign)
IMPL_ASSIGNMENT(div, DivAssign)
IMPL_ASSIGNMENT(mod, ModAssign)
    
void gen_bytecode_pop(struct ast_node* node, struct vm* v) {
    (void)node;
    START_PROFILING();
    u8 code = ByteCodePop;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_push_name(ast_node* node, vm* v) {
    START_PROFILING();
    u8 code = ByteCodePushName;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_ptr(&node->tok->data.val.string, v);
}

void gen_bytecode_push_const(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeConstant);
    START_PROFILING();
    u8 code = ByteCodePushConst;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_const(&node->tok->data.val, node->tok->data.type, primitive_size_map[node->tok->data.type], v);
}

void gen_bytecode_ref_iden(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeReferenceIdentifier);
    START_PROFILING();
    u8 code = ByteCodeRefIden;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_ptr(&node->tok->data.val.string, v);
    ast_reference* ref = get_ast_true_type(node);
    if (ref->id) {
        ref->id->gen_bytecode(ref->id, v);
    }
    if (ref->next) {
        ref->next->gen_bytecode(ref->next, v);
    }
}

void gen_bytecode_access_iden(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeReferenceIdentifier);
    START_PROFILING();
    u8 code = ByteCodeAccessIden;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_ptr(&node->tok->data.val.string, v);
    ast_reference* ref = get_ast_true_type(node);
    if (ref->id) {
        ref->id->gen_bytecode(ref->id, v);
    }
    if (ref->next) {
        ref->next->gen_bytecode(ref->next, v);
    }
}

void gen_bytecode_initvar(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeVarDecl);
    ast_vardecl* vardecl = get_ast_true_type(node);

    vardecl->expr->gen_bytecode(vardecl->expr, v);
    // vardecl->variable_name->gen_bytecode(vardecl->variable_name, v);
    gen_bytecode_push_name(vardecl->variable_name, v);

    START_PROFILING();
    u8 code = ByteCodeInitVar;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_funcparam(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeFuncParam);
    ast_funcparam* param = get_ast_true_type(node);
    if (param->next_param) {
        gen_bytecode_push_name(param->next_param, v);
        param->next_param->gen_bytecode(param->next_param, v);
    }
}

void gen_bytecode_funcdef(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeFuncDef);
    gen_bytecode_push_name(node, v);

    ast_funcdef* def = get_ast_true_type(node);
    def->param->gen_bytecode(def->param, v);

    START_PROFILING();
    u8 code = ByteCodePushConst;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_const(&def->param_count, PrimitiveDataTypeInt8, 1, v);

    code = ByteCodeFuncDef;
    vector_push(v->code, code);
}

void gen_bytecode_funcend(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeFuncEnd);
    START_PROFILING();
    u8 code = ByteCodeFuncEnd;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_arguments(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeArgs);
    ast_arg* arg = get_ast_true_type(node);
    arg->expr->gen_bytecode(arg->expr, v);
    if (arg->next_arg) {
        arg->next_arg->gen_bytecode(arg->next_arg, v);
    }
}

void gen_bytecode_funcall(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeFuncall);

    ast_funcall* funcall = get_ast_true_type(node);
    ast_arg* arg = get_ast_true_type(funcall->args);
    if (arg->expr) {
        funcall->args->gen_bytecode(funcall->args, v);
    }

    START_PROFILING();
    u8 code = ByteCodePushConst;
    vector_push(v->code, code);
    END_PROFILING(__func__);
    gen_const(&funcall->args_count, PrimitiveDataTypeInt8, 1, v);

    code = ByteCodeFuncall;
    vector_push(v->code, code);
}

void gen_bytecode_return(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeReturn);
    ast_return* ret = get_ast_true_type(node);
    if (ret->expr) {
        ret->expr->gen_bytecode(ret->expr, v);
        u8 code = ByteCodeReturn;
        vector_push(v->code, code);
        return;
    }
    u8 code = ByteCodeReturnNone;
    vector_push(v->code, code);
}

