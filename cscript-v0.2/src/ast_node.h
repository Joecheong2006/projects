#ifndef _AST_NODE_H_
#define _AST_NODE_H_
#include "core/defines.h"

typedef enum {
    AstNodeTypeEnd,
    AstNodeTypeNegate,
    AstNodeTypeConstant,
    AstNodeTypeIdentifier,
    AstNodeTypeArgs,

    AstNodeTypeExprBracket,
    AstNodeTypeExprAdd,
    AstNodeTypeExprSub,
    AstNodeTypeExprMul,
    AstNodeTypeExprDiv,
    AstNodeTypeExprMod,
    AstNodeTypeAssignment,
    AstNodeTypeAddAssign,
    AstNodeTypeSubAssign,
    AstNodeTypeMulAssign,
    AstNodeTypeDivAssign,
    AstNodeTypeModAssign,
    AstNodeTypeExprEqual,
    AstNodeTypeExprNotEqual,
    AstNodeTypeExprGreaterThan,
    AstNodeTypeExprLessThan,
    AstNodeTypeExprGreaterThanEqual,
    AstNodeTypeExprLessThanEqual,

    AstNodeTypePop,
    AstNodeTypeReferenceIdentifier,
    AstNodeTypeFuncParam,
    AstNodeTypeFuncDef,
    AstNodeTypeFuncEnd,
    AstNodeTypeVarDecl,
    AstNodeTypeIf,
    AstNodeTypeWhile,
    AstNodeTypeFuncall,
    AstNodeTypeReturn,
} AstNodeType;

struct token;
struct vm;
typedef struct ast_node ast_node;
struct ast_node {
    void(*gen_bytecode)(ast_node*,struct vm*);
    void(*destroy)(ast_node*);
    struct token* tok;
    AstNodeType type;
};

typedef struct {
    ast_node *lhs, *rhs;
} ast_binary_expression;

typedef struct {
    ast_node* expr;
} ast_expression_bracket;

typedef struct {
    ast_node* name;
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
    ast_node* next_arg;
} ast_arg;

typedef struct {
    ast_node* next_param;
} ast_funcparam;

typedef struct {
    ast_node* param;
    i32 param_count;
} ast_funcdef;

typedef struct {
    ast_node* args;
    i32 args_count;
} ast_funcall;

typedef struct {
    ast_node* expr;
} ast_return;

ast_node* make_ast_node(AstNodeType type, u64 type_size, struct token* tok, void(*destroy)(ast_node*), void(*gen_bytecode)(ast_node*,struct vm*));
void* get_ast_true_type(ast_node* node);

ast_node* make_ast_expression_bracket(struct token* tok);
ast_node* make_ast_binary_expression_add(struct token* tok);
ast_node* make_ast_binary_expression_minus(struct token* tok);
ast_node* make_ast_binary_expression_multiply(struct token* tok);
ast_node* make_ast_binary_expression_divide(struct token* tok);
ast_node* make_ast_binary_expression_modulus(struct token* tok);

ast_node* make_ast_assignment(struct token* tok);
ast_node* make_ast_add_assign(struct token* tok);
ast_node* make_ast_sub_assign(struct token* tok);
ast_node* make_ast_mul_assign(struct token* tok);
ast_node* make_ast_div_assign(struct token* tok);
ast_node* make_ast_mod_assign(struct token* tok);

ast_node* make_ast_negate(struct token* tok);
ast_node* make_ast_constant(struct token* tok);

ast_node* make_ast_reference_identifier(struct token* tok);
ast_node* make_ast_access_identifier(struct token* tok);

ast_node* make_ast_pop(struct token* tok);
ast_node* make_ast_vardecl(struct token* tok);
ast_node* make_ast_args(struct token* tok);
ast_node* make_ast_funcparam(struct token* tok);
ast_node* make_ast_funcdef(struct token* tok);
ast_node* make_ast_funcend(struct token* tok);
ast_node* make_ast_funcall(struct token* tok);
ast_node* make_ast_return(struct token* tok);

#endif
