#ifndef _PARSER_H_
#define _PARSER_H_
#include "ast_node.h"
#include "lexer.h"
#include "error_info.h"

struct token;
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
