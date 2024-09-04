#ifndef _PARSER_H_
#define _PARSER_H_
#include "ast_node.h"
#include "lexer.h"
#include "error_info.h"

typedef enum {
    ParserStateInit,
    ParserStateParsing,
    ParserStateParsingFuncBody,
    ParserStateParsingIfBody,
    ParserStateParsinElIfBody,
    ParserStateParsinElseBody,
} ParserState;

struct token;
typedef struct {
    vector(token) tokens;
    vector(error_info) errors;
    ParserState state;
    i32 pointer;
} parser;

void init_parser(parser* par, vector(token) tokens);
void free_parser(parser* par);
token* parser_peek_token(parser* par, i32 n);
void parser_report_error(parser* par, token* tok, const char* msg);

vector(ast_node*) parser_parse(parser* par);
void free_ast(vector(ast_node*) ast);

#endif
