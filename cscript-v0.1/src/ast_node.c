#include "ast_node.h"
#include "core/assert.h"
#include "container/memallocate.h"
#include "interpreter.h"

INLINE ast_node* make_ast_node(AstNodeType type, u64 type_size, struct token* tok, void(*destroy)(ast_node*), struct command*(*gen_command)(ast_node*)) {
    ast_node* node = CALLOC(1, type_size + sizeof(ast_node));
    node->type = type;
    node->gen_command = gen_command;
    node->destroy = destroy;
    node->tok = tok;
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

static void destroy_ast_access(ast_node* node) {
    ASSERT(node->type == AstNodeTypeReference);
    ast_reference* iden = get_ast_true_type(node);
    if (iden->next) {
        iden->next->destroy(iden->next);
    }
    iden->id->destroy(iden->id);
    FREE(node);
}

ast_node* make_ast_reference_funcall(struct token* tok) {
    return make_ast_node(AstNodeTypeReference, sizeof(ast_reference), tok, destroy_ast_access, make_command_reference_funcall);
}

ast_node* make_ast_reference_identifier(struct token* tok) {
    ast_node* result =  make_ast_node(AstNodeTypeReference, sizeof(ast_reference), tok, destroy_ast_access, make_command_reference_identifier);
    ast_reference* access = get_ast_true_type(result);
    access->id = make_ast_node(AstNodeTypeIdentifier, 0, tok, destroy_default, make_command_access_identifier);
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
    return make_ast_node(AstNodeTypeVarDecl, sizeof(ast_vardecl), tok, destroy_ast_vardecl, make_command_vardecl);
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
    return make_ast_node(AstNodeTypeArgs, sizeof(ast_arg), tok, destroy_ast_args, make_command_argument);
}

static void destroy_ast_funcall(ast_node* node) {
    ASSERT(node->type == AstNodeTypeFuncall);
    ast_funcall* funcall = get_ast_true_type(node);
    funcall->args->destroy(funcall->args);
    FREE(node);
}

ast_node* make_ast_funcall(struct token* tok) {
    return make_ast_node(AstNodeTypeFuncall, sizeof(ast_funcall), tok, destroy_ast_funcall, make_command_funcall);
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
    return make_ast_node(AstNodeTypeFuncParam, sizeof(ast_funcparam), tok, destroy_ast_funcparam, make_command_funcparam);
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
    return make_ast_node(AstNodeTypeFuncDef, sizeof(ast_funcdef), tok, destroy_ast_funcdef, make_command_funcdef);
}

