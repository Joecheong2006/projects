#include "bytecode.h"
#include "ast_node.h"
#include "core/assert.h"
#include "tracing.h"
#include "lexer.h"
#include "vm.h"
#include <string.h>

static void gen_ptr(const void* ptr, vm* v) {
    u32 end = vector_size(v->code);
    vector_resize(v->code, end + 8);
    memcpy(v->code + end, ptr, 8);
}

static void gen_const(ast_node* node, vm* v) {
    u8 type = node->tok->data.type; 
    vector_push(v->code, type);

    u32 end = vector_size(v->code);
    vector_resize(v->code, end + 8);
    memcpy(v->code + end, &node->tok->data.val, 8);
}

void gen_bytecode_push_name(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeReferenceIdentifier);
    START_PROFILING();
    u8 code = ByteCodePushName;
    vector_push(v->code, code);
    gen_ptr(node->tok->data.val.string, v);
    END_PROFILING(__func__);
}

void gen_bytecode_push_const(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeConstant);
    START_PROFILING();
    u8 code = ByteCodePushConst;
    vector_push(v->code, code);
    gen_const(node, v);
    END_PROFILING(__func__);
}

void gen_bytecode_ref_iden(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeReferenceIdentifier);
    START_PROFILING();
    u8 code = ByteCodeRefIden;
    vector_push(v->code, code);
    gen_ptr(node->tok->data.val.string, v);
    END_PROFILING(__func__);
}

void gen_bytecode_bracket(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeExprBracket);
    ast_expression_bracket* bracket = get_ast_true_type(node);
    bracket->expr->gen_bytecode(bracket->expr, v);
}

void gen_bytecode_add(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeExprAdd);
    START_PROFILING();
    ast_binary_expression* expr = get_ast_true_type(node);
    expr->lhs->gen_bytecode(expr->lhs, v);
    expr->rhs->gen_bytecode(expr->rhs, v);
    u8 code = ByteCodeAdd;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_minus(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeExprMinus);
    START_PROFILING();
    ast_binary_expression* expr = get_ast_true_type(node);
    expr->lhs->gen_bytecode(expr->lhs, v);
    expr->rhs->gen_bytecode(expr->rhs, v);
    u8 code = ByteCodeSub;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_multiply(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeExprMultiply);
    START_PROFILING();
    ast_binary_expression* expr = get_ast_true_type(node);
    expr->lhs->gen_bytecode(expr->lhs, v);
    expr->rhs->gen_bytecode(expr->rhs, v);
    u8 code = ByteCodeMul;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_divide(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeExprDivide);
    START_PROFILING();
    ast_binary_expression* expr = get_ast_true_type(node);
    expr->lhs->gen_bytecode(expr->lhs, v);
    expr->rhs->gen_bytecode(expr->rhs, v);
    u8 code = ByteCodeDiv;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_modulus(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeExprModulus);
    START_PROFILING();
    ast_binary_expression* expr = get_ast_true_type(node);
    expr->lhs->gen_bytecode(expr->lhs, v);
    expr->rhs->gen_bytecode(expr->rhs, v);
    u8 code = ByteCodeMod;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_negate(struct ast_node* node, struct vm* v) {
    ASSERT(node->type == AstNodeTypeNegate);
    START_PROFILING();
    ast_negate* neg = get_ast_true_type(node);
    neg->term->gen_bytecode(neg->term, v);
    u8 code = ByteCodeNegate;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

void gen_bytecode_initvar(ast_node* node, vm* v) {
    ASSERT(node->type == AstNodeTypeVarDecl);
    START_PROFILING();
    ast_vardecl* vardecl = get_ast_true_type(node);

    vardecl->expr->gen_bytecode(vardecl->expr, v);
    gen_bytecode_push_name(vardecl->variable_name, v);

    u8 code = ByteCodeInitVar;
    vector_push(v->code, code);
    END_PROFILING(__func__);
}

