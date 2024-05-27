#include "parser.h"
#include "keys_define.h"
#include <string.h>
#include "lexer.h"
#include "basic/memallocate.h"

static INLINE i32 is_assigment_operator(token* tok) { return is_operator(tok) && (tok->name_location >= OperatorAssignementBegin && tok->name_location <= OperatorAssignmentEnd); }
static INLINE i32 is_arithmetic_operator(OperatorType type) { return type >= OperatorMinus && type <= OperatorDivision; }

INLINE i32 is_node_number(NodeType type) { return type >= NodeDecNumber && type <= NodeBinNumber; }

INLINE void set_parse_error(parser* par, i32 error) {
    if (par->error != ParseErrorNoError)
        return;
    par->error = error;
}

INLINE token* parser_peek(parser* par, i32 location) {
    return par->tokens + par->index + location;
}

INLINE token* parser_peekpre(parser* par, i32 location) {
    return par->tokens + par->tokens_len + location;
}

tree_node* make_tree_node(NodeType type, i32 object_type, char* name, i32 name_len) {
    tree_node* node = MALLOC(sizeof(tree_node));
    memcpy(node, &(tree_node) {
        .type = type,
        .object_type = object_type,
        .name = name,
        .name_len = name_len,
        .nodes = make_vector(),
    }, sizeof(tree_node));
    return node;
}

void free_node(tree_node* node) {
    free_vector(&node->nodes);
    FREE(node);
}

void print_node(tree_node* node) {
    printf("node ");
    switch (node->type) {
    case NodeVariable: printf("variable"); break;
    case NodeOperator: printf("operator"); break;
    case NodeNegateOperator: printf("negate operator\n"); return; break;
    case NodeAssignmentOperator: printf("assignement operator"); break;
    case NodeDecNumber:
    case NodeHexNumber:
    case NodeOctNumber:
    case NodeBinNumber: printf("number"); break;
    default: break;
    }
    putchar(' ');
    print_token(&(token) {
                .name = node->name,
                .name_len = node->name_len,
            });
    putchar('\n');
}

void bfs(tree_node* root, void(*act)(tree_node*)) {
    act(root);
    for_vector(root->nodes, i, 0) {
        bfs(root->nodes[i], act);
    }
}

void dfs(tree_node* root, void(*act)(tree_node*)) {
    for_vector(root->nodes, i, 0) {
        dfs(root->nodes[i], act);
    }
    act(root);
}

static void set_scope_location(parser* par, tree_node* node) {
    node->scope_level = par->scope_level;
    node->scope_id = par->scope_id;
}

static tree_node* parse_variable(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeVariable, tok->name_location, tok->name, tok->name_len);
}

static tree_node* try_parse_number(parser* par) {
    token* tok = parser_peek(par, 0);
    switch (tok->type) {
    case TokenDecLiteral: {
        if (is_real_number(tok)) {
            return make_tree_node(NodeDecNumber, KeywordFloat, tok->name, tok->name_len);
        }
        return make_tree_node(NodeDecNumber, KeywordInt, tok->name, tok->name_len);
    }
    case TokenHexLiteral: return make_tree_node(NodeHexNumber, KeywordInt, tok->name, tok->name_len);
    case TokenOctLiteral: return make_tree_node(NodeOctNumber, KeywordInt, tok->name, tok->name_len);
    case TokenBinLiteral: return make_tree_node(NodeBinNumber, KeywordInt, tok->name, tok->name_len);
    default: return NULL;
    }
}

static tree_node* parse_operator(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeOperator, tok->name_location, tok->name, tok->name_len);
}

static tree_node* parse_negate_operator(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeNegateOperator, tok->name_location, tok->name, tok->name_len);
}

static tree_node* try_parse_expression_with_bracket(parser* par);

static tree_node* try_parse_expression_lhs(parser* par) {
    if (is_identifier(parser_peek(par, 0))) {
        return parse_variable(par);
    }
    else if (parser_peek(par, 0)->type == TokenOpenRoundBracket) {
        ++par->index;
        return try_parse_expression_with_bracket(par);
    }
    else if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
        return try_parse_number(par);
    }
    else if (is_operator_type(parser_peek(par, 0), OperatorMinus)) {
        tree_node* lhs = parse_negate_operator(par);
        ++par->index;
        if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
            vector_push(lhs->nodes, try_parse_number(par));
        }
        else if (parser_peek(par, 0)->type == TokenOpenRoundBracket) {
            ++par->index;
            vector_push(lhs->nodes, try_parse_expression_with_bracket(par));
        }
        if (!lhs->nodes[0]) {
            set_parse_error(par, ParseErrorMissingLhs);
            return NULL;
        }
        return lhs;
    }
    else {
        set_parse_error(par, parser_peek(par, 0)->type == TokenCloseRoundBracket
                ? ParseErrorExpectedExpression : ParseErrorMissingCloseBracket);
        return NULL;
    }
}

static tree_node* try_parse_expression_with_bracket(parser* par) {
    tree_node* lhs = try_parse_expression_lhs(par);

    if (!lhs) {
        set_parse_error(par, ParseErrorMissingLhs);
        return NULL;
    }

    ++par->index;
    tree_node* operator = NULL;

    if (parser_peek(par, 0)->type == TokenCloseRoundBracket) {
        return lhs;
    }
    if (parser_peek(par, 0)->type == TokenNewLine || parser_peek(par, 0)->type == TokenSemicolon || par->index == par->tokens_len) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingCloseBracket);
        return NULL;
    }

    if (is_operator(parser_peek(par, 0))) {
        operator = parse_operator(par);
    }

    if (!operator) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingOperator);
        return NULL;
    }

    i32 has_negate_operator = 0;
    if (operator->object_type == OperatorMinus) {
        operator->object_type = OperatorPlus;
        operator->name = "+";
        has_negate_operator = 1;
    }

    vector_pushe(operator->nodes, lhs);
    ++par->index;
    i32 has_bracket = parser_peek(par, 0)->type == TokenOpenRoundBracket;
    tree_node* rhs = try_parse_expression_with_bracket(par);
    if (!rhs) {
        set_parse_error(par, ParseErrorMissingRhs);
        dfs(operator, free_node);
        return NULL;
    }

    if (rhs->type != NodeNegateOperator && vector_size(rhs->nodes) > 0 && is_arithmetic_operator(operator->object_type) && operator->object_type > rhs->object_type) {
        if (has_bracket && vector_size(rhs->nodes[0]->nodes) == 0) {
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        vector_pushe(operator->nodes, rhs->nodes[0]);
        rhs->nodes[0] = operator;
        return rhs;
    }

    if (has_negate_operator) {
        if ((has_bracket && vector_size(rhs->nodes[0]->nodes) > 0) ||
            (vector_size(rhs->nodes) != 0 && !has_bracket)) {
            tree_node* negate = parse_negate_operator(par);
            vector_pushe(negate->nodes, rhs->nodes[0]);
            rhs->nodes[0] = negate;
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        tree_node* negate = parse_negate_operator(par);
        vector_pushe(negate->nodes, rhs);
        vector_pushe(operator->nodes, negate);
        return operator;
    }

    vector_pushe(operator->nodes, rhs);
    return operator;
}

static tree_node* try_parse_expression(parser* par) {
    tree_node* lhs = try_parse_expression_lhs(par);

    if (!lhs) {
        set_parse_error(par, ParseErrorMissingLhs);
        return NULL;
    }

    ++par->index;
    tree_node* operator = NULL;

    if (parser_peek(par, 0)->type == TokenNewLine || parser_peek(par, 0)->type == TokenSemicolon || par->index == par->tokens_len) {
        return lhs;
    }

    if (parser_peek(par, 0)->type == TokenCloseRoundBracket) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingOpenBracket);
        return NULL;
    }

    if (is_operator(parser_peek(par, 0))) {
        operator = parse_operator(par);
    }

    if (!operator) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingOperator);
        return NULL;
    }

    i32 has_negate_operator = 0;
    if (operator->object_type == OperatorMinus) {
        operator->object_type = OperatorPlus;
        operator->name = "+";
        has_negate_operator = 1;
    }

    vector_pushe(operator->nodes, lhs);
    ++par->index;
    i32 has_bracket = parser_peek(par, 0)->type == TokenOpenRoundBracket;
    tree_node* rhs = try_parse_expression(par);
    if (!rhs) {
        set_parse_error(par, ParseErrorMissingRhs);
        dfs(operator, free_node);
        return NULL;
    }

    if (rhs->type != NodeNegateOperator && vector_size(rhs->nodes) > 0 && is_arithmetic_operator(operator->object_type) && operator->object_type > rhs->object_type) {
        if (has_bracket && vector_size(rhs->nodes[0]->nodes) == 0) {
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        vector_pushe(operator->nodes, rhs->nodes[0]);
        rhs->nodes[0] = operator;
        return rhs;
    }

    if (has_negate_operator) {
        if ((has_bracket && vector_size(rhs->nodes[0]->nodes) > 0) ||
            (vector_size(rhs->nodes) != 0 && !has_bracket)) {
            tree_node* negate = parse_negate_operator(par);
            vector_pushe(negate->nodes, rhs->nodes[0]);
            rhs->nodes[0] = negate;
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        tree_node* negate = parse_negate_operator(par);
        vector_pushe(negate->nodes, rhs);
        vector_pushe(operator->nodes, negate);
        return operator;
    }

    vector_pushe(operator->nodes, rhs);
    return operator;
}

static i32 is_vailed_data_type(token* tok) {
    (void)tok;
    return 1;
}

static i32 try_parse_data_type(parser* par) {
    ++par->index;
    token* tok = parser_peek(par, 0);
    if (tok->type == TokenKeyword && is_vailed_data_type(tok)) {
        return tok->name_location;
    }
    return 0;
}

static i32 try_parse_type_dec(parser* par) {
    ++par->index;
    if (is_separator_type(parser_peek(par, 0), SeparatorColon)) {
        return try_parse_data_type(par);
    }
    return 0;
}

static tree_node* try_parse_identifier(parser* par) {
    tree_node* var = parse_variable(par);
    set_scope_location(par, var);

    if ((var->object_type = try_parse_type_dec(par)) == 0) {
        set_parse_error(par, ParseErrorMissingSeparator);
        return NULL;
    }

    ++par->index;
    if (is_assigment_operator(parser_peek(par, 0))) {
        token* assign_tok = parser_peek(par, 0);
        tree_node* assign_operator = make_tree_node(NodeAssignmentOperator, assign_tok->name_location, assign_tok->name, assign_tok->name_len);
        par->index += 1;
        tree_node* expression = try_parse_expression(par);
        if (!expression) {
            free_node(var);
            free_node(assign_operator);
            return NULL;
        }
        vector_pushe(var->nodes, assign_operator);
        vector_pushe(var->nodes[0]->nodes, expression);
        return var;
    }
    return NULL;
}

tree_node* try_parse_function_parameter(parser* par) {
    if (parser_peek(par, 0)->type == TokenCloseRoundBracket)
        return make_tree_node(NodeEmpty, -1, "", -1);
    tree_node* param = NULL;
    return param;
}

tree_node* try_parse_functin_return_type(parser* par) {
    if (parser_peek(par, 0)->type == TokenNewLine)
        return make_tree_node(NodeEmpty, -1, "", -1);
    return NULL;
}

static tree_node* try_parse_function(parser* par) {
    token* tok = parser_peek(par, 0);
    tree_node* node = make_tree_node(NodeVariable, tok->name_location, tok->name, tok->name_len);
    ++par->index;
    if (parser_peek(par, 0)->type != TokenOpenRoundBracket)
        return NULL;
    ++par->index;
    vector_push(node->nodes, try_parse_function_parameter(par));
    return NULL;
}

static tree_node* try_parse_keyword(parser* par) {
    token* tok = parser_peek(par, 0);
    switch (tok->name_location) {
    case KeywordFunction: {
        return try_parse_function(par);
    } break;
    default: return NULL;
    }
}

void init_parser(parser* par, vector(token) tokens) {
    par->scope_id = 0;
    par->scope_level = 0;
    par->index = 0;
    par->error = ParseErrorNoError;
    par->tokens = tokens;
    par->tokens_len = vector_size(tokens);
}

tree_node* parser_parse(parser* par) {
    switch (par->tokens[0].type) {
    case TokenIdentifier: {
        return try_parse_identifier(par);
    } break;
    case TokenKeyword: {
        return try_parse_keyword(par);
    } break;
    default:
        break;
    }
    return NULL;
}

