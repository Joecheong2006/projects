#ifndef _AST_NODE_H_
#define _AST_NODE_H_
#include "core/defines.h"

typedef enum {
    AstNodeTypeEnd,
    AstNodeTypeNegate,
    AstNodeTypeConstant,
    AstNodeTypeIdentifier,
    AstNodeTypeArgs,

    AstNodeTypeExpr,
    AstNodeTypeExprAdd,
    AstNodeTypeExprMinus,
    AstNodeTypeExprMultiply,
    AstNodeTypeExprDivide,
    AstNodeTypeExprModulus,
    AstNodeTypeAssignment,
    AstNodeTypeAddAssign,
    AstNodeTypeMinusAssign,
    AstNodeTypeMultiplyAssign,
    AstNodeTypeDivideAssign,
    AstNodeTypeModulusAssign,

    AstNodeTypeFuncDef,
    AstNodeTypeAssign,
    AstNodeTypeVarDecl,
    AstNodeTypeIf,
    AstNodeTypeWhile,
    AstNodeTypeFuncall,
} AstNodeType;

struct command;
struct token;
typedef struct ast_node ast_node;
struct ast_node {
    struct command*(*gen_command)(ast_node*);
    void(*destroy)(ast_node*);
    struct token* tok;
    AstNodeType type;
};

typedef struct {
    ast_node *lhs, *rhs;
} ast_binary_expression;

typedef struct {
    ast_node* variable_name;
    ast_node* expr;
} ast_assignment;

typedef struct {
    ast_node* term;
} ast_negate;

typedef struct {
    ast_node* next;
    ast_node* id;
} ast_reference;

typedef struct {
    ast_node* variable_name;
    ast_node* expr;
} ast_vardecl;

typedef struct {
    ast_node* expr;
    ast_node* next_param;
} ast_arg;

typedef struct {
    ast_node* args;
} ast_funcall;

ast_node* make_ast_node(AstNodeType type, u64 type_size, struct token* tok, void(*destroy)(ast_node*), struct command*(*gen_command)(ast_node*));
void* get_ast_true_type(ast_node* node);

ast_node* make_ast_binary_expression(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*));
ast_node* make_ast_assignment(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*));
ast_node* make_ast_negate(struct token* tok);
ast_node* make_ast_constant(struct token* tok);
ast_node* make_ast_reference_funcall(struct token* tok);
ast_node* make_ast_reference_identifier(struct token* tok);
ast_node* make_ast_vardecl(struct token* tok);
ast_node* make_ast_param(struct token* tok);
ast_node* make_ast_funcall(struct token* tok);

#endif
