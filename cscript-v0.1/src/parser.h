#ifndef _PARSER_H_
#define _PARSER_H_
#include "lexer.h"

typedef enum {
    AstNodeTypeEnd,
    AstNodeTypeTerm,
    AstNodeTypeNegate,

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

typedef struct ast_node ast_node;
typedef primitive_data(*ast_procedure)(ast_node*);
struct ast_node {
    ast_node *lhs, *rhs;
    ast_procedure procedure;
    token* tok;
    AstNodeType type;
};
void ast_tree_free(ast_node* node);

typedef struct {
    vector(token) tokens;
    vector(error_info) errors;
    i32 pointer;
} parser;

void parser_init(parser* par, vector(token) tokens);
void parser_free(parser* par);
token* parser_peek_token(parser* par, i32 n);
void parser_report_error(parser* par, token* tok, const char* msg);

vector(ast_node*) parser_parse(parser* par);

#endif
