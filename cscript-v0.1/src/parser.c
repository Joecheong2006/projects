#include "parser.h"
#include "command.h"

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
// <operator>   ::= "-" | "+" | "*" | "/"
// <assignment> ::= <member> ("+=" | "-=" | "*=" | "/=") <expr>
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

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ')') {
        return node;
    }
    ast_tree_free(node);
    parser_report_error(par, tok, "expected )");
    return NULL;
}

ast_node* expr_default_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != ')' && (tok->type == ';' || tok->type == '\n' || tok->type == TokenTypeEOF)) {
        if (tok->type != ')') {
            ++par->pointer;
        }
        return node;
    }
    ast_tree_free(node);
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
        ast_tree_free(expr);
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
        case '\n':
        case TokenTypeEOF: {
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
    return make_ast_node(AstNodeTypeIdentifier, tok, gen_command_access_identifier);
}

ast_node* parse_member(parser* par) {
    ast_node* id = parse_identifier(par);
    ast_node* result = id;
    while (1) {
        token* tok = parser_peek_token(par, 0);
        if (tok->type != '.') {
            result->type = AstNodeTypeMember;
            result->gen_command = gen_command_access_member;
            return result;
        }
        ast_node* af = parse_identifier(par);
        id = id->lhs = af;
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
        return make_ast_node(AstNodeTypeConstant, tok, gen_command_ret);
    }
    case '(':
        return parse_expr_with_brackets(par);
    case '-': {
        ++par->pointer;
        ast_node* neg = make_ast_node(AstNodeTypeExpr, tok, gen_command_negate);
        neg->lhs = parse_term(par);
        return neg;
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
        return make_ast_node(AstNodeTypeExprAdd, tok, gen_command_add);
    }
    case '-': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMinus, tok, gen_command_minus);
    }
    case '*': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMultiply, tok, gen_command_multiply);
    }
    case '/': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprDivide, tok, gen_command_divide);
    }
    default:
        return NULL;
    }
}

i32 bottom_up_need_to_reround(AstNodeType current, AstNodeType previous) {
    switch (current) {
    case AstNodeTypeExprDivide: {
    case AstNodeTypeExprMultiply:
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
            ast_tree_free(ope);
            ast_tree_free(ret);
            return NULL;
        }
        
        if (bottom_up_need_to_reround(ope->type, lhs->type)) {
            ope->rhs = rhs;
            ope->lhs = lhs->rhs;
            lhs->rhs = ope;
            continue;
        }

        ope->lhs = lhs;
        ope->rhs = rhs;
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

    ast_node* vardecl = make_ast_node(AstNodeTypeVarDecl, tok, gen_command_vardecl);
    vardecl->lhs = parse_identifier(par);
    if (!vardecl->lhs) {
        ast_tree_free(vardecl);
        return NULL;
    }

    tok = parser_peek_token(par, 0);
    if ((i32)tok->type != '=') {
        parser_report_error(par, tok, "missing equal sign");
        ast_tree_free(vardecl);
        return NULL;
    }
    ++par->pointer;

    vardecl->rhs = parse_expr(par);
    if (!vardecl->rhs) {
        ast_tree_free(vardecl);
        return NULL;
    }
    return vardecl;
}

static void clean_up_fatal(parser* par, vector(ast_node*) node) {
    for_vector(node, i, 0) {
        if (!node[i])
            continue;
        ast_tree_free(node[i]);
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
        switch ((i32)tok->type) {
        case TokenTypeKeywordVar: {
            ast_node* node = parse_vardecl(par);
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
