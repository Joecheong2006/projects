#include "parser.h"
#include "container/string.h"
#include "interpreter.h"
#include "lexer.h"
#include "tracing.h"
#include "core/log.h"

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
// <literal>    ::= <int> | <float> | char | string | "true" | "false"
// <operator>   ::= "-" | "+" | "*" | "/" | "%" | "<" | ">" | "<=" | ">=" | "==" | "!="
// <term>       ::= <literal> | <reference> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term>
// <expr>       ::= <term> {<operator> <expr>}
// <assignment> ::= <reference> ("=" | "+=" | "-=" | "*=" | "/=") <expr>
// <argument>   ::= [<expr> {"," <expr>}]
// <return>     ::= "return" [<expr>]
// <funcdef>    ::= "fun" <identifier> "(" [<identifier> {"," <identifier>}] ")" "end"
// <funcall>    ::= <reference> "(" <argument> ")"
// <vardecl>    ::= "var" <identifier> "=" <expr>
// <statement>  ::= (<vardecl> | <funcall> | <assignment>) <end>
// <if>         ::= "if" <expr> do {<statement>} ["end"]
// <elif>       ::= "elif" <expr> do {<statement>} ["end"]
// <else>       ::= "else" {<statement>} "end"
// <while>      ::= "while" <expr> do {<statement>} "end"

static void omit_separator(parser* par);
static ast_node* parse_identifier(parser* par);
static ast_node* parse_rvalue(parser* par);
static ast_node* parse_argument(parser* par);
static ast_node* parse_funcall(parser* par);
static ast_node* parse_reference(parser* par);
static ast_node* parse_term(parser* par);
static ast_node* parse_expr_with_brackets(parser* par);
static ast_node* parse_operator(parser* par);
static ast_node* parse_expr(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_exprln(parser* par);
static ast_node* parse_vardecl(parser* par);
static ast_node* parse_funcparam(parser* par);
static vector(ast_node*) parse_funcbody(parser* par);
static ast_node* parse_return(parser* par);
static ast_node* parse_funcdef(parser* par);
static ast_node* parse_assignment_operator(parser* par);
static ast_node* parse_identifier_statement(parser* par);

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_params_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);

static void clean_up_fatal(vector(ast_node*) node);
static ast_node* parser_parse_ins(parser* par);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ')') {
        END_PROFILING(__func__)
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected )");
    END_PROFILING(__func__)
    return NULL;
}

ast_node* expr_params_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ',' || tok->type == ')') {
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected , or )");
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
        parser_report_error(par, tok, "missing operator (");
    }
    else {
        parser_report_error(par, tok, "expected ; or \\n at end of expr");
    }
    return NULL;
}

ast_node* parse_expr_with_brackets(parser* par) {
    START_PROFILING()
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
    END_PROFILING(__func__)
    return expr;
}

void omit_separator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        return;
    }
    while (1) {
        switch ((i32)tok->type) {
        case ';': 
        case '\t': 
        case '\n': {
            ++par->pointer;
            tok = parser_peek_token(par, 0);
            break;
        }
        default: {
            return;
        }
        }
    }
}

ast_node* parse_identifier(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        parser_report_error(par, tok, "missing identifier");
        return NULL;
    }
    ++par->pointer;
    END_PROFILING(__func__)
    return make_ast_reference_identifier(tok);
}

ast_node* parse_rvalue(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        return NULL;
    }
    ++par->pointer;

    token* tok_param = parser_peek_token(par, 0);
    if (tok_param->type == '(') {
        ast_node* funcall_node = parse_funcall(par);
        if (!funcall_node) {
            return NULL;
        }
        funcall_node->tok = tok;
        ast_node* ref_funcall = make_ast_reference_funcall(tok);
        ast_reference* ref = get_ast_true_type(ref_funcall);
        ref->id = funcall_node;
        END_PROFILING(__func__)
        return ref_funcall;
    }
    END_PROFILING(__func__)
    return make_ast_reference_identifier(tok);
}

ast_node* parse_argument(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        return NULL;
    }
    ++par->pointer;

    ast_node* param_node = make_ast_param(tok);
    ast_node* ret = param_node;

    if (parser_peek_token(par, 0)->type == ')') {
        ++par->pointer;
        END_PROFILING(__func__)
        return ret;
    }

    while (1) {
        ast_arg* param = get_ast_true_type(param_node);
        param->expr = parse_expr(par, expr_params_terminal);
        if (!param->expr) {
            ret->destroy(ret);
            return NULL;
        }
        tok = parser_peek_token(par, 0);
        if (tok->type == ')') {
            ++par->pointer;
            END_PROFILING(__func__)
            return ret;
        }
        else if (tok->type == ',') {
            ++par->pointer;
            param->next_arg = make_ast_param(tok);
            param_node = param->next_arg;
            continue;
        }
        ret->destroy(ret);
        parser_report_error(par, tok, "expected ) or ,");
        return NULL;
    }
}

static ast_node* parse_funcall(parser* par) {
    ast_node* args = parse_argument(par);
    if (!args) {
        return NULL;
    }
    ast_node* node = make_ast_funcall(parser_peek_token(par, 0));
    ast_funcall* funcall = get_ast_true_type(node);
    funcall->args = args;
    return node;
}

ast_node* parse_reference(parser* par) {
    START_PROFILING()
    ast_node* id = parse_rvalue(par);
    ast_node* result = id;
    while (1) {
        token* tok = parser_peek_token(par, 0);
        if (tok->type == '.') {
            ++par->pointer;
            ast_node* af = parse_rvalue(par);
            ast_reference* iden = get_ast_true_type(id);
            iden->next = af;
            id = af;
        }
        else if (tok->type == '(') {
            ast_node* funcall_node = parse_funcall(par);
            if (!funcall_node) {
                return NULL;
            }
            tok->val.string = ".ret"; // TODO: put literal .ret in somewhere else
            funcall_node->tok = tok;
            ast_node* ref_funcall = make_ast_reference_funcall(tok);
            ast_reference* ref = get_ast_true_type(ref_funcall);
            ref->id = funcall_node;
            ast_reference* prev = get_ast_true_type(id);
            prev->next = ref_funcall;
            id = ref_funcall;
        }
        else {
            END_PROFILING(__func__)
            return result;
        }
    }
}

ast_node* parse_term(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeIdentifier: {
        END_PROFILING(__func__)
        return parse_rvalue(par);
    }
    case TokenTypeLiteralInt: case TokenTypeLiteralString: case TokenTypeLiteralFloat: {
        ++par->pointer;
        END_PROFILING(__func__)
        return make_ast_constant(tok);
    }
    case '(': {
        END_PROFILING(__func__)
        return parse_expr_with_brackets(par);
    }
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
    case TokenTypeOperatorEqual:
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprEqual, tok, make_command_cmp_equal);
    case TokenTypeOperatorNotEqual:
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprNotEqual, tok, make_command_cmp_not_equal);
    case '>':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprGreaterThan, tok, make_command_cmp_greater_than);
    case '<':
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprLessThan, tok, make_command_cmp_less_than);
    case TokenTypeOperatorGreaterThan:
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprGreaterThanEqual, tok, make_command_cmp_greater_than_equal);
    case TokenTypeOperatorLessThan:
        ++par->pointer;
        return make_ast_binary_expression(AstNodeTypeExprLessThanEqual, tok, make_command_cmp_less_than_equal);
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
    default: {
        return 0;
    }
    }
}

ast_node* parse_expr(parser* par, ast_node*(*is_terminal)(parser*,ast_node*)) {
    ast_node* lhs = parse_term(par);
    if (!lhs) {
        return NULL;
    }

    START_PROFILING()
    ast_node* ret = lhs;
    while (1) {
        ast_node* ope = parse_operator(par);
        if (!ope) {
            END_PROFILING(__func__);
            return is_terminal(par, ret);
        }
        if (ope->type >= AstNodeTypeExprEqual && ope->type <= AstNodeTypeExprLessThanEqual) {
            ast_node* boolean_rhs = parse_expr(par, is_terminal);
            if (!boolean_rhs) {
                lhs->destroy(lhs);
                ope->destroy(ope);
                return NULL;
            }
            ast_binary_expression* expr_ope = get_ast_true_type(ope);
            expr_ope->rhs = boolean_rhs;
            expr_ope->lhs = lhs;
            return ope;
            ret = lhs = ope;
            continue;
        }
        ast_node* rhs = parse_term(par);
        if (!rhs) {
            ope->destroy(ope);
            lhs->destroy(lhs);
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

ast_node* parse_exprln(parser* par) {
    return parse_expr(par, expr_default_terminal);
}

ast_node* parse_vardecl(parser* par) {
    START_PROFILING()
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
    END_PROFILING(__func__)
    return node;
}

static ast_node* parse_funcparam(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        parser_report_error(par, tok, "expected (");
        return NULL;
    }
    ++par->pointer;

    ast_node* param = make_ast_funcparam(tok);
    ast_node* result = param;

    if (parser_peek_token(par, 0)->type == ')') {
        ++par->pointer;
        return result;
    }
    while (1) {
        token* tok = parser_peek_token(par, 0);
        if (tok->type != TokenTypeIdentifier) {
            result->destroy(result);
            parser_report_error(par, tok, "expected parameter");
            return NULL;
        }
        ++par->pointer;
        ast_funcparam* funcparam = get_ast_true_type(param);
        param = funcparam->next_param = make_ast_funcparam(tok);

        token* sep = parser_peek_token(par, 0);
        if (sep->type == ',') {
            ++par->pointer;
            continue;
        }
        else if (sep->type == ')') {
            ++par->pointer;
            return result;
        }
        result->destroy(result);
        parser_report_error(par, sep, "expected )");
        return NULL;
    }
}

static vector(ast_node*) parse_funcbody(parser* par) {
    token* tok = parser_peek_token(par, 0);
    vector(ast_node*) result = make_vector(ast_node*);
    while (tok->type != TokenTypeKeywordEnd) {
        par->state = ParserStateParsingFuncBody;
        ast_node* ins = parser_parse_ins(par);
        if (ins == NULL) {
            clean_up_fatal(result);
            result = NULL;
            return NULL;
        }
        vector_push(result, ins);

        omit_separator(par);
        tok = parser_peek_token(par, 0);
    }
    ++par->pointer;
    par->state = ParserStateParsing;
    return result;
}

static ast_node* parse_return(parser* par) {
    ++par->pointer;
    token* tok = parser_peek_token(par, 0);
    ast_node* node = make_ast_return(tok);
    if (!node) {
        return NULL;
    }
    ast_return* ret = get_ast_true_type(node);
    ret->expr = NULL;
    if (tok->type != '\n' && tok->type != ';') {
        ret->expr = parse_exprln(par);
    }
    return node;
}

static ast_node* parse_funcdef(parser* par) {
    ++par->pointer;
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        parser_report_error(par, tok, "missing function name");
        return NULL;
    }
    ++par->pointer;
    ast_node* param = parse_funcparam(par);
    if (!param) {
        return NULL;
    }
    omit_separator(par);

    vector(ast_node*) body = parse_funcbody(par);
    if (!body) {
        param->destroy(param);
        return NULL;
    }

    ast_node* result = make_ast_funcdef(tok);
    ast_funcdef* def = get_ast_true_type(result);
    def->param = param;
    def->body = body;
    return result;
}

static ast_node* parse_assignment_operator(parser* par) {
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
        // parser_report_error(par, tok, "expected assignment operator");
        return NULL;
    }
}

static ast_node* parse_identifier_statement(parser* par) { 
    START_PROFILING()
    ast_node* refs = parse_reference(par);
    if (!refs) {
        return NULL;
    }

    ast_node* node = parse_assignment_operator(par);
    if (!node) {
        END_PROFILING(__func__)
        return refs;
    }

    ast_assignment* assignment = get_ast_true_type(node);

    assignment->expr = parse_exprln(par);
    if (!assignment->expr) {
        node->destroy(node);
        refs->destroy(refs);
        return NULL;
    }
    assignment->variable_name = refs;
    END_PROFILING(__func__)
    return node;
}

static void clean_up_fatal(vector(ast_node*) node) {
    for_vector(node, i, 0) {
        if (!node[i])
            continue;
        node[i]->destroy(node[i]);
    }
    free_vector(node);
}

static ast_node* parser_parse_ins(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch (tok->type) {
    case TokenTypeKeywordVar: {
        return parse_vardecl(par);
    }
    case TokenTypeIdentifier: {
        return parse_identifier_statement(par);
    }
    case TokenTypeKeywordFun: {
        return parse_funcdef(par);
    }
    case TokenTypeKeywordRet: {
        if (par->state == ParserStateParsing) {
            parser_report_error(par, tok, "unexpected return");
            return NULL;
        }
        return parse_return(par);
    }
    case TokenTypeEOF: {
        if (par->state != ParserStateParsing) {
            parser_report_error(par, tok, "missing end");
            return NULL;
        }
        parser_report_error(par, tok, "function declaration missing end");
        return NULL;
    }
    default: {
        parser_report_error(par, tok, "invalid token");
        return NULL;
    }
    }
}

vector(ast_node*) parser_parse(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        return NULL;
    }
    vector(ast_node*) result = make_vector(ast_node*);
    par->state = ParserStateParsing;
    while (tok) {
        ast_node* ins = parser_parse_ins(par);
        if (ins == NULL) {
            clean_up_fatal(result);
            for_vector(par->tokens, i, 0) {
                if (par->tokens[i].type == TokenTypeIdentifier) {
                    free_string(par->tokens[i].val.string);
                }
            }
            result = NULL;
            return NULL;
        }
        vector_push(result, ins);

        omit_separator(par);
        tok = parser_peek_token(par, 0);
        if (tok->type == TokenTypeEOF) {
            return result;
        }
    }
    return result;
}

void init_parser(parser* par, vector(token) tokens) {
    START_PROFILING()
    par->tokens = tokens;
    par->errors = make_vector(error_info);
    par->pointer = 0;
    par->state = ParserStateInit;
    END_PROFILING(__func__)
}

void free_parser(parser* par) {
    START_PROFILING()
    free_vector(par->tokens);
    free_vector(par->errors);
    END_PROFILING(__func__)
}

void parser_report_error(parser* par, token* tok, const char* msg) {
    error_info info = {tok->line, tok->count, msg};
    vector_push(par->errors, info);
}

token* parser_peek_token(parser* par, i32 n) {
    if (par->pointer + n < (i32)vector_size(par->tokens)) {
        return par->tokens + par->pointer + n;
    }
    return NULL;
}

void free_ast(vector(ast_node*) ast) {
    START_PROFILING()
    for_vector(ast, i, 0) {
        ast[i]->destroy(ast[i]);
    }
    free_vector(ast);
    END_PROFILING(__func__)
}

