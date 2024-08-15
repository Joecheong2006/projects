#include <string.h>
#include "lexer.h"
#include "container/memallocate.h"

// TODO(Aug15th): starting to extend parser and start up design object struct

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
// <identifer>  ::= <letter> | "_" {(<letter> | <digit> | "_")}
// <literal>    ::= <int> | <float> | char | string
// <term>       ::= <literal> | <identifier> | "(" <expr> ")" | <funcall> | "-" <term> | "+" <term>
// <expr>       ::= <term> {<operator> <expr>}
// <operator>   ::= "-" | "+" | "*" | "/"
// <params>     ::= <identifier> {"," <identifier>}
// <funcparams> ::= [<params>]
// <funcdef>    ::= "fun" <identifier> "(" <funcparams> ")" "end"
// <funcall>    ::= <identifier> "(" <funcparams> ")"
// <assign>     ::= <identifer> "=" (<expr> | <term>)
// <vardecl>    ::= "var" <identifier> "=" <expr>
// <statement>  ::= <vardecl> | <funcall> | <assign> <end>
// <if>         ::= "if" <expr> do {<statement>} ["end"]
// <elif>       ::= "elif" <expr> do {<statement>} ["end"]
// <else>       ::= "else" {<statement>} "end"
// <while>      ::= "while" <expr> do {<statement>} "end"

typedef struct {
    token* tok;
    const char* msg;
} error_info;

typedef enum {
    AstNodeTypeEnd,
    AstNodeTypeTerm,
    AstNodeTypeNegate,

    AstNodeTypeExpr,
    AstNodeTypeExprAdd,
    AstNodeTypeExprMinus,
    AstNodeTypeExprMultiply,
    AstNodeTypeExprDivide,

    AstNodeTypeFuncDef,
    AstNodeTypeAssign,
    AstNodeTypeVarDecl,
    AstNodeTypeIf,
    AstNodeTypeWhile,
} AstNodeType;

typedef struct ast_node ast_node;
typedef primitive_data(*ast_procedure)(ast_node*);
struct ast_node {
    ast_node *lhs, *rhs;
    ast_procedure procedure;
    primitive_data val;
    AstNodeType type;
};

primitive_data ast_procedure_null(ast_node* node) {
    (void)node;
    return (primitive_data) {
        .string = NULL,
    };
}

primitive_data ast_procedure_ret(ast_node* node) {
    return node->val;
}

primitive_data ast_procedure_add(ast_node* node) {
    primitive_data a = node->lhs->procedure(node->lhs);
    primitive_data b = node->rhs->procedure(node->rhs);
    return primitive_data_add(&a, &b);
}

primitive_data ast_procedure_minus(ast_node* node) {
    primitive_data a = node->lhs->procedure(node->lhs);
    primitive_data b = node->rhs->procedure(node->rhs);
    return primitive_data_minus(&a, &b);
}

primitive_data ast_procedure_multiply(ast_node* node) {
    primitive_data a = node->lhs->procedure(node->lhs);
    primitive_data b = node->rhs->procedure(node->rhs);
    return primitive_data_multiply(&a, &b);
}

primitive_data ast_procedure_divide(ast_node* node) {
    primitive_data a = node->lhs->procedure(node->lhs);
    primitive_data b = node->rhs->procedure(node->rhs);
    return primitive_data_divide(&a, &b);
}

primitive_data ast_procedure_negate(ast_node* node) {
    primitive_data a = node->lhs->procedure(node->lhs);
    return primitive_data_negate(&a);
}

typedef struct {
    vector(token) tokens;
    vector(error_info) errors;
    i32 pointer;
} parser;

void parser_init(parser* par, vector(token) tokens) {
    par->tokens = tokens;
    par->errors = make_vector();
    par->pointer = 0;
}

void parser_free(parser* par) {
    for_vector(par->tokens, i, 0) {
        if (par->tokens[i].type == TokenTypeIdentifier) {
            free_string(par->tokens[i].val.string);
        }
    }
    free_vector(par->tokens);
    free_vector(par->errors);
}

void parser_report_error(parser* par, token* tok, const char* msg) {
    vector_push(par->errors, tok, msg);
}

token* parser_peek_token(parser* par, i32 n) {
    if (par->pointer + n < (i32)vector_size(par->tokens)) {
        return par->tokens + par->pointer + n;
    }
    return NULL;
}

// NOTE: predefine parse function. Just put these here for now.
void ast_tree_free(ast_node* node);
i32 parse_end(parser* par);
ast_node* parse_identifier(parser* par);
ast_node* parse_term(parser* par);
ast_node* parse_expr_with_brackets(parser* par);
ast_node* parse_operator(parser* par);
ast_node* parse_expr_bottom_up(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
ast_node* parse_expr(parser* par);
ast_node* parse_vardecl(parser* par);

ast_node* expr_brackets_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type == ')')
        return node;
    ast_tree_free(node);
    parser_report_error(par, tok, "expected )");
    return NULL;
}

ast_node* expr_default_terminal(parser* par, ast_node* node) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != ')' && (tok->type == ';' || tok->type == '\n' || tok->type == TokenTypeEOF)) {
        return node;
    }
    ast_tree_free(node);
    if (tok->type == ')') {
        parser_report_error(par, tok, "missing operator ( before ;");
    }
    else {
        parser_report_error(par, tok, "expected ; or \\n at end of var decl");
    }
    return NULL;
}

ast_node* make_ast_node(AstNodeType type, primitive_data data, ast_procedure procedure) {
    ast_node* node = MALLOC(sizeof(ast_node));
    node->type = type;
    node->procedure = procedure;
    node->val = data;
    node->lhs = NULL;
    node->rhs = NULL;
    return node;
}

ast_node* make_ast_node_sign(AstNodeType type) {
    return make_ast_node(type, (primitive_data){0}, ast_procedure_null);
}

i32 parse_end(parser* par) {
    token* tok = parser_peek_token(par, 0);
    return tok->type == '\n' || tok->type == ';';
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

ast_node* parse_identifier(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != TokenTypeIdentifier) {
        parser_report_error(par, tok, "missing identifier");
        return NULL;
    }
    ++par->pointer;
    return make_ast_node(AstNodeTypeTerm, tok->val, ast_procedure_null);
}

ast_node* parse_term(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeIdentifier: {
        ++par->pointer;
        return make_ast_node(AstNodeTypeTerm, tok->val, ast_procedure_null);
    }
    case TokenTypeLiteralInt32: case TokenTypeLiteralString: case TokenTypeLiteralFloat32: {
        ++par->pointer;
        return make_ast_node(AstNodeTypeTerm, tok->val, ast_procedure_ret);
    }
    case '(':
        return parse_expr_with_brackets(par);
    case '-': {
        ++par->pointer;
        ast_node* neg = make_ast_node(AstNodeTypeExpr, tok->val, ast_procedure_negate);
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
        return make_ast_node(AstNodeTypeExprAdd, tok->val, ast_procedure_add);
    }
    case '-': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMinus, tok->val, ast_procedure_minus);
    }
    case '*': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMultiply, tok->val, ast_procedure_multiply);
    }
    case '/': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprDivide, tok->val, ast_procedure_divide);
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
            ast_tree_free(lhs);
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

    ast_node* vardecl = make_ast_node_sign(AstNodeTypeVarDecl);
    vardecl->lhs = parse_identifier(par);
    if (!vardecl->lhs) {
        FREE(vardecl);
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

void ast_tree_free(ast_node* node) {
    if (node == NULL)
        return;
    ast_tree_free(node->lhs);
    ast_tree_free(node->rhs);
    FREE(node);
}

void print_ast_tree(ast_node* node) {
    if (node == NULL)
        return;
    print_ast_tree(node->lhs);
    print_ast_tree(node->rhs);
    printf("type %d ", node->type);
    if (node->type == AstNodeTypeTerm) {
        printf("%d\n", node->val.int32);
    }
    else {
        printf("\n");
    }
}

int main(void) {
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char text[] = "1-(1-1-1-1-1)-1-3";
    // const char text[] = "var a = 1-1-1--3 * 3";
    const char text[] = "(2+4*(3/(.2*10))+3-1-1)*1.1+1";
    lexer lex = {text, sizeof(text) - 1, 1, 1, 0};

    parser par;
    parser_init(&par, generate_tokens(&lex));

    for_vector(par.tokens, i, 0) {
        char buf[100];
        sprintf(buf, "%d:%d:%d ", par.tokens[i].line, par.tokens[i].count, par.tokens[i].type);
        printf("%s %*c", buf, 14 - (i32)strlen(buf), ' ');
        if (par.tokens[i].type == TokenTypeLiteralInt32) {
            printf("i32: %d\n", par.tokens[i].val.int32);
        }
        else if (par.tokens[i].type == TokenTypeLiteralFloat32) {
            printf("f32: %g\n", par.tokens[i].val.float32);
        }
        else if (par.tokens[i].type == TokenTypeIdentifier) {
            printf("id:  %s len: %d\n", par.tokens[i].val.string, (i32)strlen(par.tokens[i].val.string));
        }
        else {
            if (par.tokens[i].type < 256)
                printf("sym: '%c' asc: %d\n", par.tokens[i].type == '\n' ? ' ' : par.tokens[i].type, par.tokens[i].type);
            else
                printf("key: %s\n", TokenTypeString[par.tokens[i].type - 256]);
        }
    }

    ast_node* node = parse_expr(&par);
    if (node) {
        primitive_data d = node->procedure(node);
        // printf("ret %d\n", d.int32);
        printf("ret %g\n", d.float32);
    }
    ast_tree_free(node);

    for_vector(par.errors, i, 0) {
        printf("%d:%d %s\n", par.errors[i].tok->line, par.errors[i].tok->count, par.errors[i].msg);
    }

    parser_free(&par);
    // free_vector(lex.ctx);

    printf("leak count = %d\n", check_memory_leak());
    return 0;
}
