#include "parser.h"
#include "keys_define.h"
#include <string.h>
#include "memallocate.h"

INLINE i32 is_assigment_operator(token* tok) { return is_operator(tok) && (tok->name_location >= OperatorAssignementBegin && tok->name_location <= OperatorAssignmentEnd); }

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

tree_node* make_tree_node(NodeType type, i32 object_type, const char* name, i32 name_len) {
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
    case NodeNumber: printf("number"); break;
    default: break;
    }
    putchar(' ');
    print_token_name(&(token) {
                .name = node->name,
                .name_len = node->name_len,
            });
    putchar('\n');
}

void bfs(tree_node* root, action func) {
    func(root);
    for_vector(root->nodes, i, 0) {
        bfs(root->nodes[i], func);
    }
}

void dfs(tree_node* root, action func) {
    for_vector(root->nodes, i, 0) {
        dfs(root->nodes[i], func);
    }
    func(root);
}

void free_tree(tree_node* node) {
    free_node(node);
}

tree_node* try_parse_identifier(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeVariable, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_number(parser* par) {
    token* tok = parser_peek(par, 0);
    if (is_real_number(tok)) {
        return make_tree_node(NodeNumber, KeywordFloat, tok->name, tok->name_len);
    }
    if (is_number(tok->name[0])) {
        return make_tree_node(NodeNumber, KeywordInt, tok->name, tok->name_len);
    }
    return NULL;
}

tree_node* try_parse_operator(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeOperator, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_negate_operator(parser* par) {
    token* tok = parser_peek(par, 0);
    return make_tree_node(NodeNegateOperator, tok->name_location, tok->name, tok->name_len);
}

tree_node* try_parse_expression(parser* par);
tree_node* try_parse_expression_with_bracket(parser* par) {
    tree_node* lhs = NULL;

    if (is_identifier(parser_peek(par, 0))) {
        lhs = try_parse_identifier(par);
    }
    else if (is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket)) {
        ++par->index;
        lhs = try_parse_expression_with_bracket(par);
    }
    else if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
        lhs = try_parse_number(par);
    }
    else if (is_operator_type(parser_peek(par, 0), OperatorMinus)) {
        lhs = try_parse_negate_operator(par);
        ++par->index;
        if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
            vector_push(lhs->nodes, try_parse_number(par));
        }
        else if (is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket)) {
            ++par->index;
            vector_push(lhs->nodes, try_parse_expression_with_bracket(par));
        }
        if (!lhs->nodes[0]) {
            set_parse_error(par, ParseErrorMissingLhs);
            return NULL;
        }
    }
    else {
        set_parse_error(par, ParseErrorMissingToken);
        return NULL;
    }

    if (!lhs) {
        set_parse_error(par, ParseErrorMissingLhs);
        return NULL;
    }

    ++par->index;
    tree_node* operator = NULL;

    if (is_default_separator_type(parser_peek(par, 0), TokenCloseRoundBracket)) {
        return lhs;
    }
    if (parser_peek(par, 0)->type == TokenNewLine || par->index == par->tokens_len) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingToken);
        return NULL;
    }

    if (is_operator(parser_peek(par, 0))) {
        operator = try_parse_operator(par);
    }

    if (!operator) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingToken);
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
    i32 has_bracket = is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket);
    tree_node* rhs = try_parse_expression_with_bracket(par);
    if (!rhs) {
        set_parse_error(par, ParseErrorMissingRhs);
        dfs(operator, free_node);
        return NULL;
    }

    if (vector_size(rhs->nodes) > 0 && operator->object_type > rhs->object_type) {
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
            tree_node* negate = try_parse_negate_operator(par);
            vector_pushe(negate->nodes, rhs->nodes[0]);
            rhs->nodes[0] = negate;
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        tree_node* negate = try_parse_negate_operator(par);
        vector_pushe(negate->nodes, rhs);
        vector_pushe(operator->nodes, negate);
        return operator;
    }

    vector_pushe(operator->nodes, rhs);
    return operator;
}

tree_node* try_parse_expression(parser* par) {
    tree_node* lhs = NULL;

    if (is_identifier(parser_peek(par, 0))) {
        lhs = try_parse_identifier(par);
    }
    else if (is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket)) {
        ++par->index;
        lhs = try_parse_expression_with_bracket(par);
    }
    else if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
        lhs = try_parse_number(par);
    }
    else if (is_operator_type(parser_peek(par, 0), OperatorMinus)) {
        lhs = try_parse_negate_operator(par);
        ++par->index;
        if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
            vector_push(lhs->nodes, try_parse_number(par));
        }
        else if (is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket)) {
            ++par->index;
            vector_push(lhs->nodes, try_parse_expression_with_bracket(par));
        }
        if (!lhs->nodes[0]) {
            set_parse_error(par, ParseErrorMissingLhs);
            return NULL;
        }
    }
    else {
        set_parse_error(par, ParseErrorMissingToken);
        return NULL;
    }

    if (!lhs) {
        set_parse_error(par, ParseErrorMissingLhs);
        return NULL;
    }

    ++par->index;
    tree_node* operator = NULL;

    if (parser_peek(par, 0)->type == TokenNewLine || par->index == par->tokens_len) {
        return lhs;
    }

    if (is_operator(parser_peek(par, 0))) {
        operator = try_parse_operator(par);
    }

    if (!operator) {
        dfs(lhs, free_node);
        set_parse_error(par, ParseErrorMissingToken);
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
    i32 has_bracket = is_default_separator_type(parser_peek(par, 0), TokenOpenRoundBracket);
    tree_node* rhs = try_parse_expression(par);
    if (!rhs) {
        set_parse_error(par, ParseErrorMissingRhs);
        dfs(operator, free_node);
        return NULL;
    }

    if (vector_size(rhs->nodes) > 0 && operator->object_type > rhs->object_type) {
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
            tree_node* negate = try_parse_negate_operator(par);
            vector_pushe(negate->nodes, rhs->nodes[0]);
            rhs->nodes[0] = negate;
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        tree_node* negate = try_parse_negate_operator(par);
        vector_pushe(negate->nodes, rhs);
        vector_pushe(operator->nodes, negate);
        return operator;
    }

    vector_pushe(operator->nodes, rhs);
    return operator;
}

tree_node* try_parse_variable(parser* par) {
    token* var_tok = parser_peek(par, 0);
    tree_node* var = make_tree_node(NodeVariable, -1, var_tok->name, var_tok->name_len);
    if (is_assigment_operator(parser_peek(par, 1))) {
        token* assign_tok = parser_peek(par, 1);
        tree_node* assign_operator = make_tree_node(NodeAssignmentOperator, assign_tok->name_location, assign_tok->name, assign_tok->name_len);
        par->index += 2;
        tree_node* expression = try_parse_expression(par);
        if (!expression) {
            free_node(var);
            free_node(assign_operator);
            return NULL;
        }
        vector_pushe(var->nodes, assign_operator);
        vector_pushe(var->nodes[0]->nodes, expression);
    }
    return var;
}

tree_node* parser_parse(parser* par) {
    switch (par->tokens[0].type) {
    case TokenIdentifier: {
        return try_parse_variable(par);
    } break;
    default:
        break;
    }
    return NULL;
}

