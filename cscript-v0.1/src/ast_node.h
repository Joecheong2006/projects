#ifndef _AST_NODE_H_
#define _AST_NODE_H_

typedef enum {
    AstNodeTypeEnd,
    AstNodeTypeNegate,
    AstNodeTypeConstant,
    AstNodeTypeIdentifier,
    AstNodeTypeMember,

    AstNodeTypeExpr,
    AstNodeTypeExprAdd,
    AstNodeTypeExprMinus,
    AstNodeTypeExprMultiply,
    AstNodeTypeExprDivide,

    AstNodeTypeFuncDef,
    AstNodeTypeAssign,
    AstNodeTypeVarDecl,
    AstNodeTypeIf,
    AstNodeTypeWhile,
} AstNodeType;

struct command;
struct token;
typedef struct ast_node ast_node;
struct ast_node {
    ast_node *lhs, *rhs;
    struct command*(*gen_command)(ast_node*);
    struct token* tok;
    AstNodeType type;
};

void ast_tree_free(ast_node* node);

ast_node* make_ast_node(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*));

#endif
