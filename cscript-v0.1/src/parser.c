#include "parser.h"
#include "container/string.h"
#include "interpreter.h"
#include "tracing.h"

// <digit>      ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
// <letter>     ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
// <hex>        ::= <digt> | [a-fA-F]
// <binary>     ::= "0" | "1"
// <intpart>    ::= <digit> {<digit>}
// <base2>      ::= "0b" <binary> {<binary>}
// <base16>     ::= "0x" <hex> {<hex>}
// <int>        ::= ["+" | "-"] <intpart> | <base2> | <base16>
// <float>      ::= ["+" | "-"] <intpart> | "" "." <intpart>
// <end>        ::= "\n" | ";"
// <identifier> ::= <letter> | "_" {(<letter> | <digit> | "_")}
// <rvalue>     ::= (<identifier> | <funcall>)
// <reference>  ::= <rvalue> {"." <rvalue>}
// <literal>    ::= <int> | <float> | char | string
// <term>       ::= <literal> | <reference> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term>
// <expr>       ::= <term> {<operator> <expr>}
// <operator>   ::= "-" | "+" | "*" | "/" | "%"
// <assignment> ::= <reference> ("=" | "+=" | "-=" | "*=" | "/=") <expr>
// <params>     ::= [<expr> {"," <expr>}]
// <funcdef>    ::= "fun" <identifier> "(" [<identifier> {"," <identifier>}] ")" "end"
// <funcall>    ::= <reference> "(" <funcparams> ")"
// <vardecl>    ::= "var" <identifier> "=" <expr>
// <statement>  ::= <vardecl> | <funcall> | <assignment> <end>
// <if>         ::= "if" <expr> do {<statement>} ["end"]
// <elif>       ::= "elif" <expr> do {<statement>} ["end"]
// <else>       ::= "else" {<statement>} "end"
// <while>      ::= "while" <expr> do {<statement>} "end"

static void omit_separator(parser* par);
static ast_node* parse_identifier(parser* par);
static ast_node* parse_rvalue(parser* par);
static ast_node* parse_args(parser* par);
static ast_node* parse_reference(parser* par);
static ast_node* parse_term(parser* par);
static ast_node* parse_expr_with_brackets(parser* par);
static ast_node* parse_operator(parser* par);
static ast_node* parse_expr(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_exprln(parser* par);
static ast_node* parse_vardecl(parser* par);
static ast_node* parse_assignment_operator(parser* par);
static ast_node* parse_identifier_statement(parser* par);

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_params_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ')') {
        END_PROFILING(__func__);
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected )");
    END_PROFILING(__func__);
    return NULL;
}

ast_node* expr_params_terminal(parser* par, ast_node* node) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ',' || tok->type == ')') {
        END_PROFILING(__func__);
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected ,");
    END_PROFILING(__func__);
    return NULL;
}

ast_node* expr_default_terminal(parser* par, ast_node* node) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (tok->type != ')' && (tok->type == ';' || tok->type == '\n')) {
        if (tok->type != ')') {
            ++par->pointer;
        }
        END_PROFILING(__func__);
        return node;
    }
    node->destroy(node);
    if (tok->type == ')') {
        parser_report_error(par, tok, "missing operator ( before ;");
    }
    else {
        parser_report_error(par, tok, "expected ; or \\n at end of expr");
    }
    END_PROFILING(__func__);
    return NULL;
}

ast_node* parse_expr_with_brackets(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        parser_report_error(par, tok, "missing (");
        return NULL;
    }
    ++par->pointer;
    ast_node* expr = parse_expr(par, expr_brackets_terminal);
    if (!expr) {
        return NULL;
    }

    tok = parser_peek_token(par, 0);
    if (tok->type != ')') {
        expr->destroy(expr);
        parser_report_error(par, tok, "missing )");
        return NULL;
    }
    ++par->pointer;
    // NOTE: create a new node type maybe cleaner but for now it's fine
    expr->type = AstNodeTypeExpr;
    return expr;
}

INLINE void omit_separator(parser* par) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        END_PROFILING(__func__);
        return;
    }
    while (1) {
        switch ((i32)tok->type) {
        case ';': 
        case '\n': {
            ++par->pointer;
            tok = parser_peek_token(par, 0);
            break;
        }
        default:
            END_PROFILING(__func__);
            return;
        }
    }
    END_PROFILING(__func__);
}

INLINE ast_node* parse_identifier(parser* par) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        parser_report_error(par, tok, "missing identifier");
        END_PROFILING(__func__);
        return NULL;
    }
    ++par->pointer;
    END_PROFILING(__func__);
    return make_ast_identifier(tok);
}

ast_node* parse_rvalue(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        return NULL;
    }
    ++par->pointer;

    token* tok_param = parser_peek_token(par, 0);
    if (tok_param->type == '(') {
        ast_node* args = parse_args(par);
        if (args) {
            ast_node* funcall_node = make_ast_funcall(tok);
            ast_funcall* funcall = get_ast_true_type(funcall_node);
            funcall->args = args;
            return funcall_node;
        }
    }
    return make_ast_identifier(tok);
}

ast_node* parse_args(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        return NULL;
    }
    ++par->pointer;

    ast_node* param_node = make_ast_param(tok);
    ast_node* ret = param_node;
    while (1) {
        ast_args* param = get_ast_true_type(param_node);
        param->expr = parse_expr(par, expr_params_terminal);
        if (param->expr) {
            ret->destroy(ret);
            return NULL;
        }
        tok = parser_peek_token(par, 0);
        if (tok->type == ')') {
            ++par->pointer;
            return ret;
        }
        else if (tok->type == ',') {
            ++par->pointer;
            param->next_param = make_ast_param(tok);
            param_node = param->next_param;
        }
        else {
            parser_report_error(par, tok, "expected ) or ,");
            return NULL;
        }
    }
}

ast_node* parse_reference(parser* par) {
    START_PROFILING();
    ast_node* id = parse_rvalue(par);
    ast_node* result = id;
    while (1) {
        token* tok = parser_peek_token(par, 0);
        if (tok->type == '.') {
            ++par->pointer;
            ast_node* af = parse_rvalue(par);
            ast_identifier* iden = get_ast_true_type(id);
            iden->next = af;
            id = af;
        }
        else if (tok->type == '(') {
            ++par->pointer;
            ast_node* args = parse_args(par);
            if (!args) {
                result->destroy(result);
                return NULL;
            }
            ast_node* funcall_node = make_ast_funcall(tok);
            ast_funcall* funcall = get_ast_true_type(funcall_node);
            funcall->args = args;
        }
        else {
            END_PROFILING(__func__);
            return result;
        }
    }
    END_PROFILING(__func__);
    return result;
}

ast_node* parse_term(parser* par) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeIdentifier:
        END_PROFILING(__func__);
        return parse_rvalue(par);
    case TokenTypeLiteralInt32: case TokenTypeLiteralString: case TokenTypeLiteralFloat32: {
        ++par->pointer;
        END_PROFILING(__func__);
        return make_ast_constant(tok);
    }
    case '(':
        END_PROFILING(__func__);
        return parse_expr_with_brackets(par);
    case '-': {
        ++par->pointer;
        ast_node* node = make_ast_negate(tok);
        ast_negate* neg = get_ast_true_type(node);
        neg->term = parse_term(par);
        END_PROFILING(__func__);
        return node;
    }
    case '+': {
        ++par->pointer;
        END_PROFILING(__func__);
        return parse_term(par);
    }
    default:
        parser_report_error(par, tok, "missing term");
        return NULL;
    }
}

INLINE ast_node* parse_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case '+':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprAdd, tok, make_command_add);
    case '-':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprMinus, tok, make_command_minus);
    case '*':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprMultiply, tok, make_command_multiply);
    case '/':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprDivide, tok, make_command_divide);
    case '%':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprModulus, tok, make_command_modulus);
    default:
        return NULL;
    }
}

INLINE i32 bottom_up_need_to_reround(AstNodeType current, AstNodeType previous) {
    START_PROFILING();
    switch (current) {
    case AstNodeTypeExprModulus:
    case AstNodeTypeExprDivide:
    case AstNodeTypeExprMultiply: {
        if (previous == AstNodeTypeExprAdd || previous == AstNodeTypeExprMinus) {
            END_PROFILING(__func__);
            return 1;
        }
        END_PROFILING(__func__);
        return 0;
    }
    default:
        END_PROFILING(__func__);
        return 0;
    }
}

ast_node* parse_expr(parser* par, ast_node*(*is_terminal)(parser*,ast_node*)) {
    START_PROFILING();
    ast_node* lhs = parse_term(par);
    if (!lhs) {
        return NULL;
    }

    ast_node* ret = lhs;
    while (1) {
        ast_node* ope = parse_operator(par);
        if (!ope) {
            END_PROFILING(__func__);
            return is_terminal(par, ret);
        }
        ast_node* rhs = parse_term(par);
        if (!rhs) {
            ope->destroy(ope);
            ret->destroy(ret);
            return NULL;
        }
        
        if (bottom_up_need_to_reround(ope->type, lhs->type)) {
            ast_binary_expression* expr_ope = get_ast_true_type(ope);
            ast_binary_expression* expr_lhs = get_ast_true_type(lhs);
            expr_ope->rhs = rhs;
            expr_ope->lhs = expr_lhs->rhs;
            expr_lhs->rhs = ope;
            continue;
        }

        ast_binary_expression* expr_ope = get_ast_true_type(ope);
        expr_ope->lhs = lhs;
        expr_ope->rhs = rhs;
        ret = lhs = ope;
    }
}

INLINE ast_node* parse_exprln(parser* par) {
    return parse_expr(par, expr_default_terminal);
}

ast_node* parse_vardecl(parser* par) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeKeywordVar) {
        parser_report_error(par, tok, "missing var");
        return NULL;
    }
    ++par->pointer;

    ast_node* node = make_ast_vardecl(tok);
    ast_vardecl* vardecl = get_ast_true_type(node);
    vardecl->variable_name = parse_identifier(par);
    if (!vardecl->variable_name) {
        node->destroy(node);
        return NULL;
    }

    tok = parser_peek_token(par, 0);
    if ((i32)tok->type != '=') {
        parser_report_error(par, tok, "missing equal sign");
        node->destroy(node);
        return NULL;
    }
    ++par->pointer;

    vardecl->expr = parse_exprln(par);
    if (!vardecl->expr) {
        node->destroy(node);
        return NULL;
    }
    END_PROFILING(__func__);
    return node;
}

INLINE static ast_node* parse_assignment_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((int)tok->type) {
    case TokenTypeAssignmentPlus:
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeAddAssign, tok, make_command_add_assign);
    case TokenTypeAssignmentMinus:
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeMinusAssign, tok, make_command_minus_assign);
    case TokenTypeAssignmentMultiply:
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeMultiplyAssign, tok, make_command_multiply_assign);
    case TokenTypeAssignmentDivide:
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeDivideAssign, tok, make_command_divide_assign);
    case TokenTypeAssignmentModulus:
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeModulusAssign, tok, make_command_modulus_assign);
    case '=':
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeAssignment, tok, make_command_assignment);
    default:
        parser_report_error(par, tok, "expected assignment operator");
        return NULL;
    }
}

static ast_node* parse_identifier_statement(parser* par) { 
    START_PROFILING();
    ast_node* refs = parse_reference(par);
    if (!refs) {
        return NULL;
    }

    ast_node* node = parse_assignment_operator(par);
    if (!node) {
        refs->destroy(refs);
        return NULL;
    }

    ast_assignment* assignment = get_ast_true_type(node);

    assignment->expr = parse_exprln(par);
    if (!assignment->expr) {
        node->destroy(node);
        return NULL;
    }
    assignment->variable_name = refs;
    END_PROFILING(__func__);
    return node;
}

static void clean_up_fatal(parser* par, vector(ast_node*) node) {
    for_vector(node, i, 0) {
        if (!node[i])
            continue;
        node[i]->destroy(node[i]);
    }
    free_vector(node);
    for_vector(par->tokens, i, 0) {
        if (par->tokens[i].type == TokenTypeIdentifier) {
            free_string(par->tokens[i].val.string);
        }
    }
}

vector(ast_node*) parser_parse(parser* par) {
    START_PROFILING();
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        return NULL;
    }
    vector(ast_node*) result = make_vector(ast_node*);
    while (tok) {
        switch ((i32)tok->type) {
        case TokenTypeKeywordVar: {
            ast_node* node = parse_vardecl(par);
            vector_push(result, node);
            break;
        }
        case TokenTypeIdentifier: {
            ast_node* node = parse_identifier_statement(par);
            vector_push(result, node);
            break;
        }
        default: {
            parser_report_error(par, tok, "invalid token");
            clean_up_fatal(par, result);
            return NULL;
        }
        }
        if (vector_back(result) == NULL) {
            clean_up_fatal(par, result);
            return NULL;
        }
        omit_separator(par);
        tok = parser_peek_token(par, 0);
        if (tok->type == TokenTypeEOF) {
            break;
        }
    }
    END_PROFILING(__func__);
    return result;
}

void init_parser(parser* par, vector(token) tokens) {
    START_PROFILING();
    par->tokens = tokens;
    par->errors = make_vector(error_info);
    par->pointer = 0;
    END_PROFILING(__func__);
}

void free_parser(parser* par) {
    START_PROFILING();
    free_vector(par->tokens);
    free_vector(par->errors);
    END_PROFILING(__func__);
}

INLINE void parser_report_error(parser* par, token* tok, const char* msg) {
    error_info info = {tok->line, tok->count, msg};
    vector_push(par->errors, info);
}

INLINE token* parser_peek_token(parser* par, i32 n) {
    if (par->pointer + n < (i32)vector_size(par->tokens)) {
        return par->tokens + par->pointer + n;
    }
    return NULL;
}

INLINE void free_ast(vector(ast_node*) ast) {
    for_vector(ast, i, 0) {
        ast[i]->destroy(ast[i]);
    }
    free_vector(ast);
}

