#include "parser.h"
#include "container/string.h"
#include "lexer.h"
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
// <literal>    ::= <int> | <float> | char | string | "true" | "false"
// <operator>   ::= "-" | "+" | "*" | "/" | "%" | "<" | ">" | "<=" | ">=" | "==" | "!="
// <term>       ::= <literal> | <reference> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term> | "null" | "true" | "false"
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
static ast_node* parse_rvalue_access(parser* par);
static ast_node* parse_argument(parser* par);
static ast_node* parse_funcall(parser* par);
static ast_node* parse_reference(parser* par);
static ast_node* parse_term(parser* par);
static ast_node* parse_expr_with_brackets(parser* par);
static ast_node* parse_operator(parser* par);
static ast_node* parse_expr(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_expr_statement(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_exprln(parser* par);
static ast_node* parse_vardecl(parser* par);
static ast_node* parse_funcparam(parser* par);
static ast_node* parse_return(parser* par);
static ast_node* parse_funcdef(parser* par);
static ast_node* parse_assignment_operator(parser* par);
static ast_node* parse_identifier_statement(parser* par);
static ast_node* parse_if(parser* par);

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_params_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);
static ast_node* expr_do_terminal(parser* par, ast_node* node);

static void clean_up_fatal(vector(ast_node*) node);
static ast_node* parser_parse_ins(parser* par);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type == TokenTypeKeywordAnd || tok->type == TokenTypeKeywordOr) {
        return node;
    }
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
    if (tok->type == TokenTypeKeywordAnd || tok->type == TokenTypeKeywordOr) {
        return node;
    }
    if (tok->type == ',' || tok->type == ')') {
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "expected , or )");
    return NULL;
}

ast_node* expr_default_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == TokenTypeKeywordAnd || tok->type == TokenTypeKeywordOr) {
        return node;
    }
    if (tok->type != ')' && (tok->type == ';' || tok->type == '\n' || tok->type == TokenTypeEOF)) {
        if (tok->type == ';') {
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

static ast_node* expr_do_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == TokenTypeKeywordDo) {
        return node;
    }
    node->destroy(node);
    parser_report_error(par, tok, "missing keyword do");
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
    ast_node* node = make_ast_expression_bracket(tok);
    ast_expression_bracket* bracket = get_ast_true_type(node);
    bracket->expr = expr;
    // NOTE: create a new node type maybe cleaner but for now it's fine
    // expr->type = AstNodeTypeExprBracket;
    END_PROFILING(__func__)
    return node;
}

void omit_separator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        return;
    }
    while (1) {
        switch ((i32)tok->type) {
        case ';': 
        case '\t': {
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

    ast_node* iden = make_ast_reference_identifier(tok);
    token* tok_param = parser_peek_token(par, 0);
    if (tok_param->type == '(') {
        ast_node* funcall_node = parse_funcall(par);
        if (!funcall_node) {
            return NULL;
        }
        funcall_node->tok = tok;
        ast_reference* ref = get_ast_true_type(iden);
        ref->id = funcall_node;
    }
    END_PROFILING(__func__)
    return iden;
}

static ast_node* parse_rvalue_access(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        return NULL;
    }
    ++par->pointer;

    ast_node* iden = make_ast_access_identifier(tok);
    token* tok_param = parser_peek_token(par, 0);
    if (tok_param->type == '(') {
        ast_node* funcall_node = parse_funcall(par);
        if (!funcall_node) {
            return NULL;
        }
        funcall_node->tok = tok;
        ast_reference* ref = get_ast_true_type(iden);
        ref->id = funcall_node;
    }
    END_PROFILING(__func__)
    return iden;
}

ast_node* parse_argument(parser* par) {
    START_PROFILING()
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        return NULL;
    }
    ++par->pointer;

    ast_node* arg_node = make_ast_args(tok);
    ast_node* ret = arg_node;

    if (parser_peek_token(par, 0)->type == ')') {
        ++par->pointer;
        END_PROFILING(__func__)
        return ret;
    }

    while (1) {
        ast_arg* arg = get_ast_true_type(arg_node);
        arg->expr = parse_expr(par, expr_params_terminal);
        if (!arg->expr) {
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
            arg->next_arg = make_ast_args(tok);
            arg_node = arg->next_arg;
            continue;
        }
        ret->destroy(ret);
        parser_report_error(par, tok, "expected ) or ,");
        return NULL;
    }
}

static void get_arg_count(ast_node* node, int* out) {
    ast_arg* arg = get_ast_true_type(node);
    (*out)++;
    if (arg->next_arg) {
        get_arg_count(arg->next_arg, out);
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
    funcall->args_count = 0;
    ast_arg* arg = get_ast_true_type(args);
    if (arg->expr) {
        get_arg_count(args, &funcall->args_count);
    }
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
            ast_node* af = parse_rvalue_access(par);
            ast_reference* iden = get_ast_true_type(id);
            iden->next = af;
            id = iden->next;
        }
        else if (tok->type == '(') {
            ast_node* funcall_node = parse_funcall(par);
            if (!funcall_node) {
                return NULL;
            }
            tok->data.val.string = ".ret"; // TODO: put literal .ret in somewhere else
            funcall_node->tok = tok;
            ast_node* ref_iden = make_ast_access_identifier(tok);
            ast_reference* ref = get_ast_true_type(ref_iden);
            ref->id = funcall_node;
            ast_reference* prev = get_ast_true_type(id);
            prev->next = ref_iden;
            id = ref_iden;
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
    case TokenTypeKeywordNull: {
        ++par->pointer;
        return make_ast_null(tok);
    }
    case TokenTypeKeywordTrue: {
        ++par->pointer;
        return make_ast_boolean_true(tok);
    }
    case TokenTypeKeywordFalse: {
        ++par->pointer;
        return make_ast_boolean_false(tok);
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
        return make_ast_binary_expression_add(tok);
    case '-':
        ++par->pointer;
        return make_ast_binary_expression_minus(tok);
    case '*':
        ++par->pointer;
        return make_ast_binary_expression_multiply(tok);
    case '/':
        ++par->pointer;
        return make_ast_binary_expression_divide(tok);
    case '%':
        ++par->pointer;
        return make_ast_binary_expression_modulus(tok);
    case TokenTypeOperatorEqual:
        ++par->pointer;
        return make_ast_cmp_equal(tok);
    case TokenTypeOperatorNotEqual:
        ++par->pointer;
        return make_ast_cmp_not_equal(tok);
    case '>':
        ++par->pointer;
        return make_ast_cmp_greater_than(tok);
    case '<':
        ++par->pointer;
        return make_ast_cmp_less_than(tok);
    case TokenTypeOperatorGreaterThan:
        ++par->pointer;
        return make_ast_cmp_greater_than_equal(tok);
    case TokenTypeOperatorLessThan:
        ++par->pointer;
        return make_ast_cmp_less_than_equal(tok);
    default:
        return NULL;
    }
}

i32 bottom_up_need_to_reround(AstNodeType current, AstNodeType previous) {
    switch (current) {
    case AstNodeTypeExprMod:
    case AstNodeTypeExprDiv:
    case AstNodeTypeExprMul: {
        if (previous == AstNodeTypeExprAdd || previous == AstNodeTypeExprSub) {
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
    ast_node* lhs = parse_expr_statement(par, is_terminal);
    if (!lhs) {
        return NULL;
    }
    ast_node* ret = lhs;
    while (1) {
        token* tok = parser_peek_token(par, 0);
        ast_node* ope = NULL;
        if (tok->type == TokenTypeKeywordAnd) {
            ++par->pointer;
            ope = make_ast_expr_and(tok);
        }
        else if (tok->type == TokenTypeKeywordOr) {
            ++par->pointer;
            ope = make_ast_expr_or(tok);
        }

        if (!ope) {
            return ret;
        }

        ast_binary_expression* be = get_ast_true_type(ope);
        be->lhs = lhs;
        be->rhs = parse_expr_statement(par, is_terminal);
        if (!be->rhs) {
            ope->destroy(ope);
            return NULL;
        }
        ret = lhs = ope;
    }
}

static ast_node* parse_expr_statement(parser* par, ast_node*(*is_terminal)(parser*,ast_node*)) {
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
            ast_node* boolean_rhs = parse_expr_statement(par, is_terminal);
            if (!boolean_rhs) {
                lhs->destroy(lhs);
                ope->destroy(ope);
                return NULL;
            }
            ast_binary_expression* expr_ope = get_ast_true_type(ope);
            expr_ope->rhs = boolean_rhs;
            expr_ope->lhs = lhs;
            return ope;
            // ret = lhs = ope;
            // continue;
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

static void get_funcparam_count(ast_node* node, int* out) {
    ast_funcparam* param = get_ast_true_type(node);
    if (param->next_param) {
        (*out)++;
        get_funcparam_count(param->next_param, out);
    }
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

    ast_node* result = make_ast_funcdef(tok);
    ast_funcdef* def = get_ast_true_type(result);
    def->param = param;
    def->param_count = 0;
    get_funcparam_count(param, &def->param_count);
    i32 state = ParserStateParsingFuncBody;
    vector_push(par->states, state);
    return result;
}

static ast_node* parse_assignment_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeAssignmentPlus:
        ++par->pointer;
        return make_ast_add_assign(tok);
    case TokenTypeAssignmentMinus:
        ++par->pointer;
        return make_ast_sub_assign(tok);
    case TokenTypeAssignmentMultiply:
        ++par->pointer;
        return make_ast_mul_assign(tok);
    case TokenTypeAssignmentDivide:
        ++par->pointer;
        return make_ast_div_assign(tok);
    case TokenTypeAssignmentModulus:
        ++par->pointer;
        return make_ast_mod_assign(tok);
    case '=':
        ++par->pointer;
        return make_ast_assignment(tok);
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
    assignment->name = refs;
    END_PROFILING(__func__)
    return node;
}

ast_node* parse_if(parser* par) {
    ++par->pointer;
    ast_node* expr = parse_expr(par, expr_do_terminal);
    if (!expr) {
        return NULL;
    }

    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeKeywordDo) {
        expr->destroy(expr);
        return NULL;
    }
    return NULL;
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
    switch ((i32)tok->type) {
    case TokenTypeKeywordVar: {
        return parse_vardecl(par);
    }
    case TokenTypeIdentifier: {
        return parse_identifier_statement(par);
    }
    case TokenTypeKeywordFun: {
        return parse_funcdef(par);
    }
    case TokenTypeKeywordIf: {
        return parse_if(par);
    }
    case TokenTypeKeywordEnd: {
        if (vector_back(par->states) == ParserStateParsingFuncBody) {
            vector_pop(par->states);
            par->pointer++;
            return make_ast_funcend(NULL);
        }
        parser_report_error(par, tok, "end does not match any scope");
        return NULL;
    }
    case TokenTypeKeywordRet: {
        if (vector_back(par->states) == ParserStateParsing) {
            parser_report_error(par, tok, "unexpected return");
            return NULL;
        }
        return parse_return(par);
    }
    case '\n': {
        par->pointer++;
        return make_ast_newline(tok);
    }
    case TokenTypeEOF: {
        if (vector_back(par->states) != ParserStateParsing) {
            parser_report_error(par, tok, "missing end");
            return NULL;
        }
        else if (vector_back(par->states) == ParserStateParsingFuncBody) {
            parser_report_error(par, tok, "function declaration missing end");
            return NULL;
        }
        return make_ast_eof(tok);
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
    i32 state = ParserStateParsing;
    vector_push(par->states, state);
    while (tok) {
        i32 is_reference = tok->type == TokenTypeIdentifier;
        ast_node* ins = parser_parse_ins(par);
        if (ins == NULL) {
            clean_up_fatal(result);
            for_vector(par->tokens, i, 0) {
                if (par->tokens[i].type == TokenTypeIdentifier) {
                    free_string(par->tokens[i].data.val.string);
                }
            }
            result = NULL;
            return NULL;
        }
        vector_push(result, ins);
        if (is_reference && ins->type == AstNodeTypeReferenceIdentifier) {
            ast_node* node = make_ast_pop(tok);
            vector_push(result, node);
        }

        if (tok->type == TokenTypeEOF) {
            return result;
        }
        omit_separator(par);
        tok = parser_peek_token(par, 0);
    }
    return result;
}

void init_parser(parser* par, vector(token) tokens) {
    START_PROFILING()
    par->tokens = tokens;
    par->errors = make_vector(error_info);
    par->pointer = 0;
    par->states = make_vector(ParserState);
    END_PROFILING(__func__)
}

void free_parser(parser* par) {
    START_PROFILING()
    free_vector(par->tokens);
    free_vector(par->errors);
    free_vector(par->states);
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

