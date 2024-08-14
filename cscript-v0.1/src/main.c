#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "container/memallocate.h"

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
// <term>       ::= <literal> | <identifier> | "(" <expr> ")" | <funcall>
// <expr>       ::= <term> <operator> (<term> | <expr>)
// <operator>   ::= "-" | "+" | "*" | "/"
// <params>     ::= <identifier> {"," <identifier>}
// <funcparams> ::= [<params>]
// <funcdef>    ::= "fun" <identifier> "(" <funcparams> ")" "end"
// <funcall>    ::= <identifier> "(" <funcparams> ")"
// <assign>     ::= <identifer> "=" (<expr> | <term>)
// <vardecl>    ::= var <identifier> "=" <term>
// <statement>  ::= <vardecl> | <funcall> | <assign> <end>
// <if>         ::= if (<expr> | <term>) do {<statement>} ["end"]
// <elif>       ::= elif (<expr> | <term>) do {<statement>} ["end"]
// <while>      ::= while (<expr> | <term>) do {<statement>} ["end"]

// var a = 1

i32 primitive_data_guess_type(primitive_data* a, primitive_data* b) {
    return a->type[2] > b->type[2] ? a->type[2] : b->type[2];
}

void primitive_data_cast_to(i32 type, primitive_data* pd) {
    switch (type - pd->type[2]) {
    case TokenTypeLiteralInt32 - TokenTypeLiteralFloat32: {
        pd->int32 = pd->float32;
        pd->type[2] = type;
        break;
    }
    case TokenTypeLiteralFloat32 - TokenTypeLiteralInt32: {
        pd->float32 = pd->int32;
        pd->type[2] = type;
        break;
    }
    default:
        break;
    }
}

#define IMPL_PRIMITIVE_ARITHMETIC(oper, name)\
primitive_data primitive_data_##name(primitive_data* a, primitive_data* b) {\
    i32 type = primitive_data_guess_type(a, b);\
    primitive_data result = {.type[2] = type};\
    primitive_data_cast_to(type, a);\
    primitive_data_cast_to(type, b);\
    switch (type) {\
    case TokenTypeLiteralInt32: {\
        result.int32 = a->int32 oper b->int32;\
        return result;\
    }\
    case TokenTypeLiteralFloat32: {\
        result.float32 = a->float32 oper b->float32;\
        return result;\
    }\
    default:\
        return result;\
    }\
}

IMPL_PRIMITIVE_ARITHMETIC(+, add)
IMPL_PRIMITIVE_ARITHMETIC(-, minus)
IMPL_PRIMITIVE_ARITHMETIC(*, multiply)
IMPL_PRIMITIVE_ARITHMETIC(/, divide)

primitive_data primitive_data_negate(primitive_data* a) {
    primitive_data result = {.type[2] = a->type[2]};
    switch (a->type[2]) {
    case TokenTypeLiteralInt32: {
        result.int32 = -a->int32;
        return result;
    }
    case TokenTypeLiteralFloat32: {
        result.float32 = -a->float32;
        return result;
    }
    default:
        return result;
    }
}

typedef enum {
    AstNodeTypeTerm,
    AstNodeTypeNegate,
    AstNodeTypeExpr,
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
    i32 pointer;
} parser;

token* parser_peek_token(parser* par, i32 n) {
    if (par->pointer + n < (i32)vector_size(par->tokens)) {
        return par->tokens + par->pointer + n;
    }
    return NULL;
}

// NOTE: predefine parse function
void ast_tree_free(ast_node* node);
ast_node* parse_term(parser* par);
ast_node* parse_expr_with_brackets(parser* par);
ast_node* parse_operator(parser* par);
ast_node* parse_expr_bottom_up(parser* par);

ast_node* make_ast_node(AstNodeType type, primitive_data data, ast_procedure procedure) {
    ast_node* node = MALLOC(sizeof(ast_node));
    node->type = type;
    node->procedure = procedure;
    node->val = data;
    node->lhs = NULL;
    node->rhs = NULL;
    return node;
}

ast_node* parse_expr_with_brackets(parser* par) {
    token* tok = parser_peek_token(par, 0);
    if (tok->type != '(') {
        printf("missing (\n");
        return NULL;
    }
    ++par->pointer;
    ast_node* expr = parse_expr_bottom_up(par);
    tok = parser_peek_token(par, 0);
    if (tok->type != ')') {
        ast_tree_free(expr);
        printf("missing )\n");
        return NULL;
    }
    ++par->pointer;
    return expr;
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
        return NULL;
    }
}

ast_node* parse_operator(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case '+': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExpr, tok->val, ast_procedure_add);
    }
    case '-': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeNegate, tok->val, ast_procedure_minus);
    }
    case '*': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExpr, tok->val, ast_procedure_multiply);
    }
    case '/': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExpr, tok->val, ast_procedure_divide);
    }
    default:
        return NULL;
    }
}

ast_node* parse_expr_bottom_up(parser* par) {
    ast_node* lhs = parse_term(par);
    if (lhs == NULL) {
        return NULL;
    }

    ast_node* ret = lhs;
    while (1) {
#if 1
        ast_node* ope = parse_operator(par);
        if (!ope) {
            return ret;
        }
        ast_node* rhs = parse_term(par);
        if (!rhs) {
            ast_tree_free(lhs);
            return NULL;
        }
        ret = ope;
        ret->lhs = lhs;
        ret->rhs = rhs;
        lhs = ope;
#else
        token* ope_tok = parser_peek_token(par, 0);
        if (ope_tok == NULL) {
            return ope;
        }
        switch ((i32)ope_tok->type) {
        case '+': {
            ++par->pointer;
            ast_node* rhs = parse_term(par);
            if (!rhs) {
                ast_tree_free(lhs);
                return NULL;
            }
            ope = make_ast_node(AstNodeTypeExpr, ope_tok->val, ast_procedure_add);
            ope->lhs = lhs;
            ope->rhs = rhs;
            lhs = ope;
            continue;
        }
        case '-': {
            ++par->pointer;
            ast_node* rhs = parse_term(par);
            if (!rhs) {
                ast_tree_free(lhs);
                return NULL;
            }
            ope = make_ast_node(AstNodeTypeExpr, ope_tok->val, ast_procedure_minus);
            ope->lhs = lhs;
            ope->rhs = rhs;
            lhs = ope;
            continue;
        }
        default:
            return ope;
        }
#endif
    }
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
    printf("%d %d ", node->type, node->val.type[2]);
    if (node->type == AstNodeTypeTerm) {
        printf("%d\n", node->val.int32);
    }
    else {
        printf("\n");
    }
}

// TODO(Aug15th): issue arithmetic priority
int main(void) {
    // lexer lex = {NULL, -1, 1, 1, 0};
    // lexer_load_file_text(&lex, "test.cscript");

    // const char expr[] = "2 * 5 + (1-2)";
    // const char expr[] = "1-(1-1-1-1-1)-1-3";
    const char expr[] = "-+++--+-+1";
    lexer lex = {expr, sizeof(expr) - 1, 1, 1, 0};

    parser par = { generate_tokens(&lex), 0 };

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
            free_string(par.tokens[i].val.string);
        }
        else {
            if (par.tokens[i].type < 256)
                printf("sym: '%c' asc: %d\n", par.tokens[i].type == '\n' ? ' ' : par.tokens[i].type, par.tokens[i].type);
            else
                printf("key: %s\n", TokenTypeString[par.tokens[i].type - 256]);
        }
    }

    ast_node* node = parse_expr_bottom_up(&par);
    print_ast_tree(node);

    if (node) {
        primitive_data d = node->procedure(node);
        printf("ret %d\n", d.int32);
    }

    ast_tree_free(node);

    free_vector(par.tokens);
    // free_vector(lex.ctx);

    printf("leak count = %d\n", check_memory_leak());
    return 0;
}
