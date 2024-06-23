#include "parser.h"

#include "environment.h"
#include "keys_define.h"
#include <string.h>
#include "lexer.h"
#include "basic/memallocate.h"

static INLINE i32 is_assigment_operator(token* tok);
static INLINE i32 is_arithmetic_operator(OperatorType type);
static tree_node* parse_rvariable(parser* par);
static INLINE tree_node* parse_char_literal(parser* par);
static INLINE tree_node* parse_string_literal(parser* par);
static tree_node* try_parse_number(parser* par);
static tree_node* try_parse_operator(parser* par);
static tree_node* parse_negate_operator(parser* par);
static i32 default_terminal(token* tok);
static i32 default_error_terminal(token* tok);
static i32 bracket_terminal(token* tok);
static i32 bracket_error_terminal(token* tok);
static i32 parameter_terminal(token* tok);
static i32 parameter_error_terminal(token* tok);
static tree_node* try_parse_binary_expression(parser* par, i32(*is_terminal)(token*), i32(*is_error_terminal)(token*));
static tree_node* try_parse_binary_expression_lhs(parser* par);
static tree_node* try_parse_assign_operator(parser* par);
static tree_node* try_parse_function_call_parameters(parser* par);
static tree_node* try_parse_function_call_name(parser* par);
static tree_node* try_parse_function_call(parser* par);
static tree_node* try_parse_function_parameters(parser* par);
static tree_node* try_parse_function_name(parser* par);
static tree_node* try_parse_function_decl(parser* par);
static tree_node* try_parse_identifier(parser* par);
static tree_node* try_parse_end(parser* par);
static tree_node* try_parse_keyword(parser* par);
static tree_node* try_parse_return(parser* par);
static tree_node* try_parse_type_decl(parser* par);

static INLINE i32 is_assigment_operator(token* tok) { return is_operator(tok) && (tok->sub_type >= OperatorAssignementBegin && tok->sub_type <= OperatorAssignmentEnd); }
static INLINE i32 is_arithmetic_operator(OperatorType type) { return type >= OperatorMinus && type <= OperatorDivision; }

INLINE i32 is_node_number(NodeType type) { return type >= NodeDecNumber && type <= NodeBinNumber; }

INLINE token* parser_peek(parser* par, i32 location) {
    return par->tokens + par->index + location;
}

INLINE token* parser_peekpre(parser* par, i32 location) {
    return par->tokens + par->tokens_len + location;
}

tree_node* make_tree_node() {
    tree_node* node = MALLOC(sizeof(tree_node));
    memset(node, 0, sizeof(tree_node));
    // memcpy(node, &(tree_node) {
    //     .type = type,
    //     .object_type = object_type,
    //     .name = name,
    //     .name_len = name_len,
    //     .nodes = make_vector(),
    // }, sizeof(tree_node));
    // if (tok) {
    //     memcpy(&node->val, &tok->val, sizeof(temp_data));
    // }
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
    case NodeFunctionDecl: printf("function decl"); break;
    default: break;
    }
    putchar(' ');
    print_token(&(token) {
                .type = node->object_type,
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

static tree_node* make_rvalue_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeVariable;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len;
    return result;
}

static tree_node* make_char_literal_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeCharLiteral;
    result->object_type = NodeTypeChar;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len - 2;
    return result;
}

static tree_node* make_string_literal_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeStringLiteral;
    result->object_type = NodeTypeString;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len - 2;
    return result;
}

static tree_node* make_operator_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeOperator;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len;
    return result;
}

static tree_node* make_negate_operator_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeNegateOperator;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len;
    return result;
}

static tree_node* make_end_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeEnd;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    result->name = tok->name;
    result->name_len = tok->name_len;
    return result;
}

static tree_node* make_number_literal_node(NodeType type, token* tok) {
    tree_node* result = make_tree_node();
    result->type = type;
    result->object_type = NodeTypeInt;
    result->nodes = make_vector();
    memcpy(&result->val, &tok->val, sizeof(temp_data));
    return result;
}

static tree_node* make_assign_operator_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeAssignmentOperator;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_function_parameters_node() {
    tree_node* result = make_tree_node();
    result->type = NodeFunctionParameters;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_function_parameter_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeFunctionParameter;
    result->object_type = tok->sub_type;
    result->name = tok->name;
    result->name_len = tok->name_len;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_function_decl_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeFunctionDecl;
    result->object_type = tok->sub_type;
    result->name = tok->name;
    result->name_len = tok->name_len;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_function_call_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeFunctionCall;
    result->object_type = tok->sub_type;
    result->name = tok->name;
    result->name_len = tok->name_len;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_type_decl_node(token* tok) {
    tree_node* result = make_tree_node();
    result->type = NodeTypeDecl;
    result->object_type = tok->sub_type;
    result->nodes = make_vector();
    return result;
}

static tree_node* make_return_node() {
    tree_node* result = make_tree_node();
    result->type = NodeReturn;
    result->nodes = make_vector();
    return result;
}

INLINE static tree_node* parse_rvariable(parser* par) {
    return make_rvalue_node(parser_peek(par, 0));
}

INLINE static tree_node* parse_char_literal(parser* par) {
    return make_char_literal_node(parser_peek(par, 0));
}

INLINE static tree_node* parse_string_literal(parser* par) {
    return make_string_literal_node(parser_peek(par, 0));
}

static tree_node* try_parse_number(parser* par) {
    token* tok = parser_peek(par, 0);
    switch (tok->type) {
    case TokenDecLiteral: {
        tree_node* result = make_number_literal_node(NodeDecNumber, tok);
        if (is_real_number(tok)) {
            result->object_type = NodeTypeFloat;
        }
        return result;
    }
    case TokenHexLiteral: return make_number_literal_node(NodeHexNumber, tok);
    case TokenOctLiteral: return make_number_literal_node(NodeOctNumber, tok);
    case TokenBinLiteral: return make_number_literal_node(NodeBinNumber, tok);
    default: return NULL;
    }
}

static tree_node* try_parse_operator(parser* par) {
    ++par->index;
    token* tok = parser_peek(par, 0);
    if (is_operator(parser_peek(par, 0))) {
        return make_operator_node(tok);
    }
    return NULL;
}

static tree_node* parse_negate_operator(parser* par) {
    return make_negate_operator_node(parser_peek(par, 0));
}

static INLINE i32 default_terminal(token* tok) { return tok->type == TokenNewLine || tok->type == TokenSemicolon; }
static INLINE i32 default_error_terminal(token* tok) { return tok->type == TokenCloseRoundBracket; }
static INLINE i32 bracket_terminal(token* tok) { return tok->type == TokenCloseRoundBracket; }
static INLINE i32 bracket_error_terminal(token* tok) { return tok->type == TokenNewLine || tok->type == TokenSemicolon; }

static tree_node* try_parse_binary_expression_lhs(parser* par) {
    ++par->index;
    if (is_identifier(parser_peek(par, 0))) {
        if (parser_peek(par, 1)->type == TokenOpenRoundBracket) {
            tree_node* node = try_parse_function_call(par);
            if (node) {
                return node;
            }
            add_error_message((error_message){
                .type = ParserErrorUndefineName
            });
            return NULL;
        }
        return parse_rvariable(par);
    }

    if (is_string_literal(parser_peek(par, 0))) {
        return parse_string_literal(par);
    }
    if (is_char_literal(parser_peek(par, 0))) {
        return parse_char_literal(par);
    }
    if (parser_peek(par, 0)->type == TokenOpenRoundBracket) {
        tree_node* node = try_parse_binary_expression(par, &bracket_terminal, &bracket_error_terminal);
        ++par->index;
        return node;
    }
    if (is_number(parser_peek(par, 0)->name[0]) || is_real_number(parser_peek(par, 0))) {
        return try_parse_number(par);
    }
    if (is_operator_type(parser_peek(par, 0), OperatorMinus)) {
        tree_node* lhs = parse_negate_operator(par);
        tree_node* lhs_lhs = try_parse_binary_expression_lhs(par);
        if (!lhs_lhs) {
            return NULL;
        }
        vector_push(lhs->nodes, lhs_lhs);
        return lhs;
    }
    add_error_message((error_message){
        .type = parser_peek(par, 0)->type == TokenCloseRoundBracket
            ? ParserErrorExpectedExpression : ParserErrorMissingCloseBracket
    });
    return NULL;
}

static tree_node* try_parse_binary_expression(parser* par, i32(*is_terminal)(token*), i32(*is_error_terminal)(token*)) {
    tree_node* lhs = try_parse_binary_expression_lhs(par);

    if (!lhs) {
        add_error_message((error_message){
            .type = ParserErrorMissingLhs
        });
        return NULL;
    }

    if (is_error_terminal(parser_peek(par, 1))) {
        dfs(lhs, free_node);
        add_error_message((error_message){
            .type = ParserErrorMissingOpenBracket
        });
        return NULL;
    }

    if (is_terminal(parser_peek(par, 1)) || par->index == par->tokens_len) {
        return lhs;
    }

    tree_node* operator = try_parse_operator(par);
    if (!operator) {
        dfs(lhs, free_node);
        add_error_message((error_message){
            .type = ParserErrorMissingOperator
        });
        return NULL;
    }

    i32 has_negate_operator = 0;
    if (operator->object_type == OperatorMinus) {
        operator->object_type = OperatorPlus;
        has_negate_operator = 1;
    }

    vector_pushe(operator->nodes, lhs);
    i32 has_bracket = parser_peek(par, 1)->type == TokenOpenRoundBracket;
    tree_node* rhs = try_parse_binary_expression(par, is_terminal, is_error_terminal);
    if (!rhs) {
        add_error_message((error_message){
            .type = ParserErrorMissingRhs
        });
        dfs(operator, free_node);
        return NULL;
    }

    if (rhs->type != NodeNegateOperator && rhs->type != NodeFunctionCall && vector_size(rhs->nodes) > 0 && is_arithmetic_operator(operator->object_type) && operator->object_type > rhs->object_type) {
        if (has_bracket && vector_size(rhs->nodes[0]->nodes) == 0) {
            vector_pushe(operator->nodes, rhs);
            return operator;
        }
        vector_pushe(operator->nodes, rhs->nodes[0]);
        rhs->nodes[0] = operator;
        return rhs;
    }

    if (has_negate_operator) {
        if (rhs->type != NodeFunctionCall && ((has_bracket && vector_size(rhs->nodes[0]->nodes) > 0) ||
                (vector_size(rhs->nodes) != 0 && !has_bracket))) {
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

static tree_node* try_parse_assign_operator(parser* par) {
    ++par->index;
    token* tok = parser_peek(par, 0);
    if (is_assigment_operator(tok)) {
        return make_assign_operator_node(tok);
    }
    return NULL;
}

static i32 parameter_terminal(token* tok) {
    return default_terminal(tok) || tok->type == TokenComma || tok->type == TokenCloseRoundBracket;
}

static i32 parameter_error_terminal(token* tok) {
    (void)tok;
    return 0;
}

static tree_node* try_parse_function_call_parameters(parser* par) {
    tree_node* params = make_function_parameters_node();

    ++par->index;
    if (parser_peek(par, 1)->type == TokenCloseRoundBracket) {
        ++par->index;
        return params;
    }
    while (1) {
        tree_node* param = try_parse_binary_expression(par, &parameter_terminal, &parameter_error_terminal);
        if (param) {
            vector_push(params->nodes, param);
        }
        else {
            return NULL;
        }

        ++par->index;
        token* tok = parser_peek(par, 0);
        if (tok->type == TokenCloseRoundBracket) {
            break;
        }
        else if (tok->type == TokenComma) {
            continue;
        }
        else {
            // NOTE: missing close backet
            return NULL;
        }
    }
    return params;
}

static tree_node* try_parse_function_call_name(parser* par) {
    token* tok = parser_peek(par, 0);
    if (is_identifier(tok)) {
        return make_function_call_node(tok);
    }
    --par->index;
    add_error_message((error_message){
        .type = ParserErrorInvalidName,
    });
    return NULL;
}

static tree_node* try_parse_function_call(parser* par) {
    tree_node* node = try_parse_function_call_name(par);
    tree_node* params = try_parse_function_call_parameters(par);

    if (!params) {
        return NULL;
    }
    vector_push(node->nodes, params);
    return node;
}

static tree_node* try_parse_type_decl(parser* par) {
    ++par->index;
    if (parser_peek(par, 0)->type != TokenColon) {
        --par->index;
        return NULL;
    }
    ++par->index;
    token* tok = parser_peek(par, 0);
    if (is_data_type(tok)) {
        return make_type_decl_node(tok);
    }
    --par->index;
    add_error_message((error_message){
        .type = ParserErrorInvalidName
    });
    return NULL;
}

static tree_node* try_parse_identifier(parser* par) {
    if (parser_peek(par, 1)->type == TokenOpenRoundBracket) {
        tree_node* node = try_parse_function_call(par);
        if (node) {
            return node;
        }
        add_error_message((error_message){
            .type = ParserErrorInvalidName
        });
        return NULL;
    }

    token* tok = parser_peek(par, 0);
    tree_node* var = make_rvalue_node(tok);

    tree_node* type_decl = try_parse_type_decl(par);
    if (type_decl) {
        vector_pushe(var->nodes, type_decl);
    }

    if (is_operator_type(parser_peek(par, 1), OperatorAssign)) {
        var->type = NodeVariableInitialize;
    }
    else if (is_assigment_operator(parser_peek(par, 1))) {
        var->type = NodeVariableAssignment;
    }
    else {
        add_error_message((error_message){
            .type = ParserErrorMissingAssignOperator
        });
        return NULL;
    }

    tree_node* assign_operator = try_parse_assign_operator(par);
    if (!assign_operator) {
        dfs(var, free_node);
        add_error_message((error_message){
            .type = ParserErrorMissingAssignOperator
        });
        return NULL;
    }

    tree_node* expression = try_parse_binary_expression(par, &default_terminal, &default_error_terminal);
    if (!expression) {
        dfs(var, free_node);
        return NULL;
    }

    vector_pushe(assign_operator->nodes, expression);
    vector_pushe(var->nodes, assign_operator);
    return var;
}

static tree_node* try_parse_function_parameters(parser* par) {
    tree_node* params = make_function_parameters_node();
    if (parser_peek(par, 1)->type == TokenCloseRoundBracket) {
        ++par->index;
        return params;
    }
    while (1) {
        ++par->index;
        token* tok = parser_peek(par, 0);
        if (is_identifier(tok)) {
            vector_push(params->nodes, make_function_parameter_node(tok));
        }
        else {
            add_error_message((error_message){
                .type = ParserErrorUndefineName
            });
            dfs(params, free_node);
            return NULL;
        }

        ++par->index;
        tok = parser_peek(par, 0);
        if (tok->type == TokenCloseRoundBracket) {
            break;
        }
        else if (tok->type == TokenComma) {
            continue;
        }
        else {
            // NOTE: missing close backet
            return NULL;
        }
    }
    return params;
}

static tree_node* try_parse_function_name(parser* par) {
    token* tok = parser_peek(par, 0);
    if (is_identifier(tok)) {
        return make_function_decl_node(tok);
    }
    --par->index;
    add_error_message((error_message){
        .type = ParserErrorUndefineName
    });
    return NULL;
}

static tree_node* try_parse_function_decl(parser* par) {
    ++par->index;
    tree_node* node = try_parse_function_name(par);
    if (!node) {
        return NULL;
    }
    ++par->index;
    if (parser_peek(par, 0)->type != TokenOpenRoundBracket) {
        add_error_message((error_message){
            .type = ParserErrorMissingOpenBracket
        });
        free_node(node);
        return NULL;
    }
    tree_node* params = try_parse_function_parameters(par);
    if (!params) {
        dfs(node, free_node);
        return NULL;
    }
    vector_push(node->nodes, params);
    return node;
}

static tree_node* try_parse_end(parser* par) {
    if (par->tokens_len > 2) { // end \n -> 2
        // NOTE: unkown statement after end
        return NULL;
    }
    return make_end_node(parser_peek(par, 0));
}

static tree_node* try_parse_return(parser* par) {
    tree_node* ret_node = make_return_node();
    if (parser_peek(par, 1)->type == TokenNewLine) {
        return ret_node;
    }
    tree_node* expr = try_parse_binary_expression(par, &default_terminal, &default_error_terminal);
    if (!expr) {
        return NULL;
    }
    vector_push(ret_node->nodes, expr);
    return ret_node;
}

static tree_node* try_parse_keyword(parser* par) {
    static i32 in_function = 0;
    token* tok = parser_peek(par, 0);
    switch (tok->sub_type) {
    case KeywordFunction: {
        if (in_function) {
            // NOTE: already in function
            return NULL;
        }
        in_function = 1;
        return try_parse_function_decl(par);
    } break;
    case KeywordEnd: {
        if (!in_function) {
            // NOTE: already outside function
            return NULL;
        }
        in_function = 0;
        return try_parse_end(par);
    } break;
    case KeywordReturn: {
        if (!in_function) {
            return NULL;
        }
        return try_parse_return(par);
    } break;
    default: return NULL;
    }
}

void init_parser(parser* par) {
    ASSERT_MSG(par != NULL, "invalid parser");
    memset(par, 0, sizeof(parser));
}

void parser_set_tokens(parser* par, vector(token) tokens) {
    ASSERT_MSG(par != NULL, "invalid parser");
    par->index = 0;
    par->tokens = tokens;
    par->tokens_len = vector_size(tokens);
}

tree_node* parser_parse(parser* par) {
    ASSERT_MSG(par != NULL, "invalid parser");
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

