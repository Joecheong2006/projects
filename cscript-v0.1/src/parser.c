#include "parser.h"
#include "container/string.h"
#include "command.h"
#include "tracing.h"

// <digit>      ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
// <letter>     ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
// <hex>        ::= <digt> | [a-fA-F]
// <binary>     ::= "0" | "1"
// <intpart>    ::= <digit> {<digit>}
// <base2>      ::= "0b" <binary> {<binary>}
// <base16      ::= "0x" <hex> {<hex>}
// <int>        ::= ["+" | "-"] <intpart> | <base2>
// <float>      ::= ["+" | "-"] <intpart> | "" "." <intpart>
// <end>        ::= "\n" | ";"
// <name>       ::= <letter> | "_" {(<letter> | <digit> | "_")}
// <identifer>  ::= <name> {"." <name>}
// <member>     ::= <identifier> {"." <identifier>}
// <literal>    ::= <int> | <float> | char | string
// <term>       ::= <literal> | <member> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term>
// <expr>       ::= <term> {<operator> <expr>}
// <operator>   ::= "-" | "+" | "*" | "/" | "%"
// <assignment> ::= <member> ("=" | "+=" | "-=" | "*=" | "/=") <expr>
// <params>     ::= <identifier> {"," <identifier>}
// <funcparams> ::= [<params>]
// <funcdef>    ::= "fun" <identifier> "(" <funcparams> ")" "end"
// <funcall>    ::= <member> "(" <funcparams> ")"
// <vardecl>    ::= "var" <identifier> "=" <expr>
// <statement>  ::= <vardecl> | <funcall> | <assignment> <end>
// <if>         ::= "if" <expr> do {<statement>} ["end"]
// <elif>       ::= "elif" <expr> do {<statement>} ["end"]
// <else>       ::= "else" {<statement>} "end"
// <while>      ::= "while" <expr> do {<statement>} "end"

static void omit_separator(parser* par);
static ast_node* parse_identifier(parser* par);
static ast_node* parse_member(parser* par);
static ast_node* parse_term(parser* par);
static ast_node* parse_expr_with_brackets(parser* par);
static ast_node* parse_operator(parser* par);
static ast_node* parse_expr_bottom_up(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_expr(parser* par);
static ast_node* parse_vardecl(parser* par);
static ast_node* parse_assignment_operator(parser* par);
static ast_node* parse_identifier_statement(parser* par);

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ')') {
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected )");
    return NULL;
}

ast_node* expr_default_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != ')' && (tok->type == ';' || tok->type == '\n')) {
        if (tok->type != ')') {
            ++par->pointer;
        }
        return node;
    }
    node->destroy(node);
    if (tok->type == ')') {
        parser_report_error(par, tok, "missing operator ( before ;");
    }
    else {
        parser_report_error(par, tok, "expected ; or \\n at end of expr");
    }
    return NULL;
}

ast_node* parse_expr_with_brackets(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        parser_report_error(par, tok, "missing (");
        return NULL;
    }
    ++par->pointer;
    ast_node* expr = parse_expr_bottom_up(par, expr_brackets_terminal);
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

void omit_separator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (!tok)
        return;
    while (1) {
        switch ((i32)tok->type) {
        case ';': 
        case '\n': {
            ++par->pointer;
            tok = parser_peek_token(par, 0);
            break;
        }
        default:
            return;
        }
    }
}

ast_node* parse_identifier(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        parser_report_error(par, tok, "missing identifier");
        return NULL;
    }
    ++par->pointer;
    return make_ast_identifier(tok);
}

ast_node* parse_member(parser* par) {
    ast_node* id = parse_identifier(par);
    ast_node* result = id;
    while (1) {
        token* tok = parser_peek_token(par, 0);
        if (tok->type != '.') {
            return result;
        }
        ast_node* af = parse_identifier(par);
        ast_identifier* iden = get_ast_true_type(id);
        iden->next = af;
        id = af;
    }
    return result;
}

ast_node* parse_term(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeIdentifier: {
        return parse_member(par);
    }
    case TokenTypeLiteralInt32: case TokenTypeLiteralString: case TokenTypeLiteralFloat32: {
        ++par->pointer;
        return make_ast_constant(tok);
    }
    case '(':
        return parse_expr_with_brackets(par);
    case '-': {
        ++par->pointer;
        ast_node* node = make_ast_negate(tok);
        ast_negate* neg = get_ast_true_type(node);
        neg->term = parse_term(par);
        return node;
    }
    case '+': {
        ++par->pointer;
        return parse_term(par);
    }
    default:
        parser_report_error(par, tok, "missing term");
        return NULL;
    }
}

ast_node* parse_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case '+': {
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprAdd, tok, make_command_add);
    }
    case '-': {
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprMinus, tok, make_command_minus);
    }
    case '*': {
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprMultiply, tok, make_command_multiply);
    }
    case '/': {
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprDivide, tok, make_command_divide);
    }
    case '%': {
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprModulus, tok, make_command_modulus);
    }
    default:
        return NULL;
    }
}

i32 bottom_up_need_to_reround(AstNodeType current, AstNodeType previous) {
    switch (current) {
    case AstNodeTypeExprModulus:
    case AstNodeTypeExprDivide:
    case AstNodeTypeExprMultiply: {
        if (previous == AstNodeTypeExprAdd || previous == AstNodeTypeExprMinus) {
            return 1;
        }
        return 0;
    }
    default:
        return 0;
    }
}

ast_node* parse_expr_bottom_up(parser* par, ast_node*(*is_terminal)(parser*,ast_node*)) {
    ast_node* lhs = parse_term(par);
    if (!lhs) {
        return NULL;
    }

    ast_node* ret = lhs;
    while (1) {
        ast_node* ope = parse_operator(par);
        if (!ope) {
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

ast_node* parse_expr(parser* par) {
    return parse_expr_bottom_up(par, expr_default_terminal);
}

ast_node* parse_vardecl(parser* par) {
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

    vardecl->expr = parse_expr(par);
    if (!vardecl->expr) {
        node->destroy(node);
        return NULL;
    }
    return node;
}

static ast_node* parse_assignment_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch (tok->type) {
    case TokenTypeAssignmentPlus: {
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeExprAddAssign, tok, make_command_add_assign);
    }
    case TokenTypeAssignmentMinus: {
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeExprMinusAssign, tok, NULL);
    }
    case TokenTypeAssignmentMultiply: {
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeExprMultiplyAssign, tok, NULL);
    }
    case TokenTypeAssignmentDivide: {
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeExprDivideAssign, tok, NULL);
    }
    case TokenTypeAssignmentModulus: {
        ++par->pointer;
        return make_ast_assignment(AstNodeTypeExprModulusAssign, tok, NULL);
    }
    default: {
        parser_report_error(par, tok, "expected assignment operator");
        return NULL;
    }
    }
}

static ast_node* parse_identifier_statement(parser* par) { 
    ast_node* mem = parse_member(par);
    if (!mem) {
        return NULL;
    }

    ast_node* node = parse_assignment_operator(par);
    if (!node) {
        return NULL;
    }

    ast_assignment* assignment = get_ast_true_type(node);

    assignment->expr = parse_expr(par);
    if (!assignment->expr) {
        node->destroy(node);
        return NULL;
    }
    assignment->variable_name = mem;
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
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        return NULL;
    }
    vector(ast_node*) result = make_vector(ast_node*);
    while (tok) {
        START_PROFILING();
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
            END_PROFILING(__func__);
            return NULL;
        }
        }
        if (vector_back(result) == NULL) {
            clean_up_fatal(par, result);
            END_PROFILING(__func__);
            return NULL;
        }
        END_PROFILING(__func__);
        omit_separator(par);
        tok = parser_peek_token(par, 0);
        if (tok->type == TokenTypeEOF) {
            return result;
        }
    }
    return result;
}

void parser_init(parser* par, vector(token) tokens) {
    par->tokens = tokens;
    par->errors = make_vector(error_info);
    par->pointer = 0;
}

void parser_free(parser* par) {
    free_vector(par->tokens);
    free_vector(par->errors);
}

void parser_report_error(parser* par, token* tok, const char* msg) {
    error_info info = {tok, msg};
    vector_push(par->errors, info);
}

token* parser_peek_token(parser* par, i32 n) {
    if (par->pointer + n < (i32)vector_size(par->tokens)) {
        return par->tokens + par->pointer + n;
    }
    return NULL;
}
