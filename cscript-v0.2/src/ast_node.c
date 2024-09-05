#include "ast_node.h"
#include "core/assert.h"
#include "container/memallocate.h"
#include "tracing.h"
#include "bytecode.h"

#include "lexer.h"

INLINE ast_node* make_ast_node(AstNodeType type, u64 type_size, struct token* tok, void(*destroy)(ast_node*), void(*gen_bytecode)(ast_node*,struct vm*)) {
    START_PROFILING();
    ast_node* node = CALLOC(1, type_size + sizeof(ast_node));
    node->type = type;
    node->gen_bytecode = gen_bytecode;
    node->destroy = destroy;
    node->tok = tok;
    END_PROFILING(__func__);
    return node;
}

INLINE void* get_ast_true_type(ast_node* node) { return node + 1; }

static void destroy_default(ast_node* node) {
    ASSERT(node);
    FREE(node);
}

static void destroy_ast_binary_expression(ast_node* node) {
    ASSERT(node);
    ast_binary_expression* expr = get_ast_true_type(node);
    if (expr->lhs) {
        expr->lhs->destroy(expr->lhs);
    }
    if (expr->rhs) {
        expr->rhs->destroy(expr->rhs);
    }
    FREE(node);
}

ast_node* make_ast_binary_expression_add(struct token* tok) {
    return make_ast_node(AstNodeTypeExprAdd, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_bytecode_add);
}

ast_node* make_ast_binary_expression_minus(struct token* tok) {
    return make_ast_node(AstNodeTypeExprMinus, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_bytecode_minus);
}

ast_node* make_ast_binary_expression_multiply(struct token* tok) {
    return make_ast_node(AstNodeTypeExprMultiply, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_bytecode_multiply);
}

ast_node* make_ast_binary_expression_divide(struct token* tok) {
    return make_ast_node(AstNodeTypeExprDivide, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_bytecode_divide);
}

ast_node* make_ast_binary_expression_modulus(struct token* tok) {
    return make_ast_node(AstNodeTypeExprModulus, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_bytecode_modulus);
}

static void destroy_ast_assignment(ast_node* node) {
    ASSERT(node);
    ast_assignment* assignment = get_ast_true_type(node);
    if (assignment->variable_name) {
        assignment->variable_name->destroy(assignment->variable_name);
    }
    if (assignment->expr) {
        assignment->expr->destroy(assignment->expr);
    }
    FREE(node);
}

ast_node* make_ast_assignment(AstNodeType type, struct token* tok, void(*gen_bytecode)(ast_node*,struct vm*)) {
    return make_ast_node(type, sizeof(ast_assignment), tok, destroy_ast_assignment, gen_bytecode);
}

static void destroy_ast_negate(ast_node* node) {
    ASSERT(node->type == AstNodeTypeNegate);
    ast_negate* negate = get_ast_true_type(node);
    negate->term->destroy(negate->term);
    FREE(node);
}

ast_node* make_ast_negate(struct token* tok) {
    return make_ast_node(AstNodeTypeNegate, sizeof(ast_negate), tok, destroy_ast_negate, gen_bytecode_negate);
}

ast_node* make_ast_constant(struct token* tok) {
    return make_ast_node(AstNodeTypeConstant, 0, tok, destroy_default, gen_bytecode_push_const);
}

static void destroy_ast_access_funcall(ast_node* node) {
    ASSERT(node->type == AstNodeTypeReferenceFuncall);
    ast_reference* iden = get_ast_true_type(node);
    if (iden->next) {
        iden->next->destroy(iden->next);
    }
    // iden->id->destroy(iden->id);
    FREE(node);
}

ast_node* make_ast_reference_funcall(struct token* tok) {
    return make_ast_node(AstNodeTypeReferenceFuncall, sizeof(ast_reference), tok, destroy_ast_access_funcall, NULL);
}

static void destroy_ast_access_identifier(ast_node* node) {
    ASSERT(node->type == AstNodeTypeReferenceIdentifier);
    ast_reference* iden = get_ast_true_type(node);
    if (iden->next) {
        iden->next->destroy(iden->next);
    }
    FREE(node);
}

ast_node* make_ast_reference_identifier(struct token* tok) {
    ast_node* result = make_ast_node(AstNodeTypeReferenceIdentifier, sizeof(ast_reference), tok, destroy_ast_access_identifier, gen_bytecode_ref_iden);
    return result;
}

static void destroy_ast_vardecl(ast_node* node) {
    ASSERT(node->type == AstNodeTypeVarDecl);
    ast_vardecl* vardecl = get_ast_true_type(node);
    if (vardecl->variable_name) {
        vardecl->variable_name->destroy(vardecl->variable_name);
    }
    if (vardecl->expr) {
        vardecl->expr->destroy(vardecl->expr);
    }
    FREE(node);
}

ast_node* make_ast_vardecl(struct token* tok) {
    return make_ast_node(AstNodeTypeVarDecl, sizeof(ast_vardecl), tok, destroy_ast_vardecl, gen_bytecode_initvar);
}

static void destroy_ast_args(ast_node* node) {
    ASSERT(node->type == AstNodeTypeArgs);
    ast_arg* args = get_ast_true_type(node);
    if (args->expr) {
        args->expr->destroy(args->expr);
    }
    if (args->next_arg) {
        args->next_arg->destroy(args->next_arg);
    }
    FREE(node);
}

ast_node* make_ast_param(struct token* tok) {
    return make_ast_node(AstNodeTypeArgs, sizeof(ast_arg), tok, destroy_ast_args, NULL);
}

static void destroy_ast_funcparam(ast_node* node) {
    ASSERT(node->type == AstNodeTypeFuncParam);
    ast_funcparam* param = get_ast_true_type(node);
    if (param->next_param) {
        param->next_param->destroy(param->next_param);
    }
    FREE(node);
}

ast_node* make_ast_funcparam(struct token* tok) {
    return make_ast_node(AstNodeTypeFuncParam, sizeof(ast_funcparam), tok, destroy_ast_funcparam, NULL);
}

static void destroy_ast_funcdef(ast_node* node) {
    ASSERT(node->type == AstNodeTypeFuncDef);
    ast_funcdef* def = get_ast_true_type(node);
    if (def->param) {
        def->param->destroy(def->param);
    }
    for_vector(def->body, i, 0) {
        def->body[i]->destroy(def->body[i]);
    }
    free_vector(def->body);
    FREE(node);
}

ast_node* make_ast_funcdef(struct token* tok) {
    return make_ast_node(AstNodeTypeFuncDef, sizeof(ast_funcdef), tok, destroy_ast_funcdef, NULL);
}

static void destroy_ast_funcall(ast_node* node) {
    ASSERT(node->type == AstNodeTypeFuncall);
    ast_funcall* funcall = get_ast_true_type(node);
    funcall->args->destroy(funcall->args);
    FREE(node);
}

ast_node* make_ast_funcall(struct token* tok) {
    return make_ast_node(AstNodeTypeFuncall, sizeof(ast_funcall), tok, destroy_ast_funcall, NULL);
}

static void destroy_ast_return(ast_node* node) {
    ASSERT(node->type == AstNodeTypeReturn);
    ast_return* ret = get_ast_true_type(node);
    if (ret->expr) {
        ret->expr->destroy(ret->expr);
    }
    FREE(node);
}

ast_node* make_ast_return(struct token* tok) {
    return make_ast_node(AstNodeTypeReturn, sizeof(ast_return), tok, destroy_ast_return, NULL);
}

