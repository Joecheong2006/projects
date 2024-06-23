#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef struct {
    vector(token) tokens;
    i16 index, tokens_len;
} parser;

token* parser_peek(parser* par, i32 location);
token* parser_peekpre(parser* par, i32 location);

typedef enum {
    NodeVariable,
    NodeVariableInitialize,
    NodeVariableAssignment,
    NodeEnd,
    NodeReturn,
    NodeFunctionDecl,
    NodeFunctionCall,
    NodeFunctionParameters,
    NodeFunctionParameter,
    NodeOperator,
    NodeAssignmentOperator,
    NodeTypeInt,
    NodeStringLiteral,
    NodeCharLiteral,
    NodeTypeChar,
    NodeTypeFloat,
    NodeDecNumber,
    NodeHexNumber,
    NodeOctNumber,
    NodeBinNumber,
    NodeTypeBool,
    NodeTypeString,
    NodeTypeDecl,
    NodeUserType,
    NodeTypeNull,
    NodeNegateOperator,
    NodeEmpty,
} NodeType;

typedef struct tree_node tree_node;
struct tree_node {
    i16 object_type, name_len;
    NodeType type;
    char* name;
    vector(tree_node*) nodes;
    temp_data(*eval)(tree_node*);
    temp_data val;
};

tree_node* make_tree_node();
void free_node(tree_node* node);
void print_node(tree_node* node);

void bfs(tree_node* root, void(*act)(tree_node*));
void dfs(tree_node* root, void(*act)(tree_node*));

i32 is_node_number(NodeType type);

void init_parser(parser* par);
void parser_set_tokens(parser* par, vector(token) tokens);
tree_node* parser_parse(parser* par);

#endif
