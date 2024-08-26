#include "ast_node.h"
#include "core/assert.h"
#include "container/memallocate.h"
#include "command.h"
#include "tracing.h"

INLINE ast_node* make_ast_node(AstNodeType type, u64 type_size, struct token* tok, void(*destroy)(ast_node*), struct command*(*gen_command)(ast_node*)) {
    START_PROFILING();
    ast_node* node = CALLOC(1, type_size + sizeof(ast_node));
    node->type = type;
    node->gen_command = gen_command;
    node->destroy = destroy;
    node->tok = tok;
    END_PROFILING(__func__);
    return node;
}

INLINE void* get_ast_true_type(ast_node* node) { return node + 1; }

INLINE static void destroy_default(ast_node* node) {
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

ast_node* make_ast_binary_expression(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*)) {
    return make_ast_node(type, sizeof(ast_binary_expression), tok, destroy_ast_binary_expression, gen_command);
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

ast_node* make_ast_assignment(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*)) {
    return make_ast_node(type, sizeof(ast_assignment), tok, destroy_ast_assignment, gen_command);
}

static void destroy_ast_negate(ast_node* node) {
    ASSERT(node->type == AstNodeTypeNegate);
    ast_negate* negate = get_ast_true_type(node);
    negate->term->destroy(negate->term);
    FREE(node);
}

ast_node* make_ast_negate(struct token* tok) {
    return make_ast_node(AstNodeTypeNegate, sizeof(ast_negate), tok, destroy_ast_negate, make_command_negate);
}

ast_node* make_ast_constant(struct token* tok) {
    return make_ast_node(AstNodeTypeConstant, 0, tok, destroy_default, make_command_get_constant);
}

static void destroy_ast_identifier(ast_node* node) {
    ASSERT(node->type == AstNodeTypeIdentifier);
    ast_identifier* iden = get_ast_true_type(node);
    if (iden->next) {
        iden->next->destroy(iden->next);
    }
    FREE(node);
}

ast_node* make_ast_identifier(struct token* tok) {
    return make_ast_node(AstNodeTypeIdentifier, sizeof(ast_identifier), tok, destroy_ast_identifier, make_command_access);
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
    return make_ast_node(AstNodeTypeVarDecl, sizeof(ast_vardecl), tok, destroy_ast_vardecl, make_command_vardecl);
}

