#include "parser.h"
#include "container/memallocate.h"

static void omit_separator(parser* par);
static ast_node* parse_identifier(parser* par);
static ast_node* parse_term(parser* par);
static ast_node* parse_expr_with_brackets(parser* par);
static ast_node* parse_operator(parser* par);
static ast_node* parse_expr_bottom_up(parser* par, ast_node*(*is_terminal)(parser*,ast_node*));
static ast_node* parse_expr(parser* par);
static ast_node* parse_vardecl(parser* par);

static ast_node* expr_brackets_terminal(parser* par, ast_node* node);
static ast_node* expr_default_terminal(parser* par, ast_node* node);

static primitive_data ast_procedure_null(ast_node* node);
static primitive_data ast_procedure_ret(ast_node* node);
static primitive_data ast_procedure_add(ast_node* node);
static primitive_data ast_procedure_minus(ast_node* node);
static primitive_data ast_procedure_multiply(ast_node* node);
static primitive_data ast_procedure_divide(ast_node* node);
static primitive_data ast_procedure_negate(ast_node* node);
static primitive_data ast_procedure_vardecl(ast_node* node);

primitive_data ast_procedure_null(ast_node* node) {
    (void)node;
    return (primitive_data) {
        .string = NULL,
    };
}

primitive_data ast_procedure_ret(ast_node* node) {
    return node->tok->val;
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

primitive_data ast_procedure_vardecl(ast_node* node) {
    primitive_data result = { .type[2] = 0 };
    primitive_data val = node->rhs->procedure(node->rhs);
    if (val.type[2] == -1) {
        result = val;
        return result;
    }
    else if (val.type[2] == TokenTypeLiteralInt32) {
        printf("%d\n", val.int32);
    }
    else if (val.type[2] == TokenTypeLiteralFloat32) {
        printf("%g\n", val.float32);
    }
    else {
        printf("unkown primitive_data type %d\n", val.type[2]);
    }
    return result;
}

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

ast_node* make_ast_node(AstNodeType type, token* tok, ast_procedure procedure) {
    ast_node* node = MALLOC(sizeof(ast_node));
    node->type = type;
    node->procedure = procedure;
    node->tok = tok;
    node->lhs = NULL;
    node->rhs = NULL;
    return node;
}

ast_node* make_ast_node_sign(AstNodeType type) {
    return make_ast_node(type, NULL, ast_procedure_null);
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
    return make_ast_node(AstNodeTypeTerm, NULL, ast_procedure_null);
}

ast_node* parse_term(parser* par) {
    token* tok = parser_peek_token(par, 0);
    switch ((i32)tok->type) {
    case TokenTypeIdentifier: {
        ++par->pointer;
        return make_ast_node(AstNodeTypeTerm, NULL, ast_procedure_null);
    }
    case TokenTypeLiteralInt32: case TokenTypeLiteralString: case TokenTypeLiteralFloat32: {
        ++par->pointer;
        return make_ast_node(AstNodeTypeTerm, tok, ast_procedure_ret);
    }
    case '(':
        return parse_expr_with_brackets(par);
    case '-': {
        ++par->pointer;
        ast_node* neg = make_ast_node(AstNodeTypeExpr, NULL, ast_procedure_negate);
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
        return make_ast_node(AstNodeTypeExprAdd, NULL, ast_procedure_add);
    }
    case '-': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMinus, NULL, ast_procedure_minus);
    }
    case '*': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprMultiply, NULL, ast_procedure_multiply);
    }
    case '/': {
        ++par->pointer;
        return make_ast_node(AstNodeTypeExprDivide, NULL, ast_procedure_divide);
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

    ast_node* vardecl = make_ast_node(AstNodeTypeVarDecl, NULL, ast_procedure_vardecl);
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

vector(ast_node*) parser_parse(parser* par) {
    vector(ast_node*) result = make_vector();
    token* tok = parser_peek_token(par, 0);
    if (!tok) {
        free_vector(result);
        return NULL;
    }
    while (tok) {
        switch ((i32)tok->type) {
        case TokenTypeKeywordVar: vector_push(result, parse_vardecl(par)); break;
        default: {
            parser_report_error(par, tok, "invalid token");
            for_vector(result, i, 0) {
                if (!result[i])
                    continue;
                ast_tree_free(result[i]);
            }
            free_vector(result);
            return NULL;
        }
        }
        if (vector_back(result) == NULL) {
            for_vector(result, i, 0) {
                if (!result[i])
                    continue;
                ast_tree_free(result[i]);
            }
            free_vector(result);
            return NULL;
        }
        omit_separator(par);
        tok = parser_peek_token(par, 0);
    }
    return result;
}

void ast_tree_free(ast_node* node) {
    if (node == NULL)
        return;
    ast_tree_free(node->lhs);
    ast_tree_free(node->rhs);
    FREE(node);
}

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
