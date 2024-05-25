#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef enum {
    ParseErrorNoError,
    ParseErrorMissingToken,
    ParseErrorMissingLhs,
    ParseErrorMissingRhs,
} ParseError;

typedef struct {
    vector(token) tokens;
    u64 index, tokens_len;
    ParseError error;
} parser;

void set_parse_error(parser* par, i32 error);
token* parser_peek(parser* par, i32 location);
token* parser_peekpre(parser* par, i32 location);

typedef enum {
    NodeVariable,
    NodeVariableAssign,
    NodeOperator,
    NodeAssignmentOperator,
    NodeNumber,
    NodeNegateOperator
} NodeType;

typedef struct tree_node tree_node;
struct tree_node {
    NodeType type;
    i8 object_type;
    i8 name_len;
    const char* name;
    vector(tree_node*) nodes;
};

tree_node* make_tree_node(NodeType type, i32 object_type, const char* name, i32 name_len);
void free_node(tree_node* node);
void print_node(tree_node* node);

typedef void(*action)(tree_node*);

void bfs(tree_node* root, action func);
void dfs(tree_node* root, action func);
void free_tree(tree_node* node);

tree_node* try_parse_identifier(parser* par);
tree_node* try_parse_number(parser* par);
tree_node* try_parse_operator(parser* par);
tree_node* try_parse_negate_operator(parser* par);
tree_node* try_parse_expression_with_bracket(parser* par);
tree_node* try_parse_expression(parser* par);
tree_node* parser_parse(parser* par);

#endif
