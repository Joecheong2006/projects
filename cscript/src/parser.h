#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef enum {
    ParseErrorNoError,
    ParseErrorMissingToken,
    ParseErrorMissingLhs,
    ParseErrorMissingRhs,
    ParseErrorMissingOpenBracket,
    ParseErrorMissingCloseBracket,
    ParseErrorMissingOperator,
    ParseErrorExpectedExpression,
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
    NodeDecNumber,
    NodeHexNumber,
    NodeOctNumber,
    NodeBinNumber,
    NodeNegateOperator,
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

void bfs(tree_node* root, void(*act)(tree_node*));
void dfs(tree_node* root, void(*act)(tree_node*));

i32 is_node_number(NodeType type);
tree_node* parser_parse(parser* par);

#endif
