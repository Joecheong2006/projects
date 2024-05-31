#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "basic/string.h"

typedef enum {
    ParserErrorNoError,
    ParserErrorMissingToken,
    ParserErrorMissingLhs,
    ParserErrorMissingRhs,
    ParserErrorMissingOpenBracket,
    ParserErrorMissingCloseBracket,
    ParserErrorMissingOperator,
    ParserErrorMissingAssignOperator,
    ParserErrorMissingSeparator,
    ParserErrorExpectedExpression,
    ParserErrorInvalidOperandsType,
    ParserErrorUndefineName,
} ParserError;

typedef struct {
    vector(token) tokens;
    i16 index, tokens_len;
    vector(string) error_messgaes;
} parser;

void set_parse_error(parser* par, i32 error);
token* parser_peek(parser* par, i32 location);
token* parser_peekpre(parser* par, i32 location);

typedef enum {
    NodeVariable,
    NodeVariableInitialize,
    NodeVariableAssignment,
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
    NodeTypeString,
    NodeUserType,
    NodeNegateOperator,
    NodeEmpty,
} NodeType;

typedef struct tree_node tree_node;
struct tree_node {
    char* name;
    vector(tree_node*) nodes;
    NodeType type;
    i16 object_type, name_len;
};

tree_node* make_tree_node(NodeType type, i32 object_type, char* name, i32 name_len);
void free_node(tree_node* node);
void print_node(tree_node* node);

void bfs(tree_node* root, void(*act)(tree_node*));
void dfs(tree_node* root, void(*act)(tree_node*));

i32 is_node_number(NodeType type);

void print_parser_error(parser* par);
void init_parser(parser* par);
void parser_set_tokens(parser* par, vector(token) tokens);
tree_node* parser_parse(parser* par);
void free_parser(parser* par);

#endif
