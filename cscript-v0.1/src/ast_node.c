#include "ast_node.h"
#include "container/memallocate.h"

ast_node* make_ast_node(AstNodeType type, struct token* tok, struct command*(*gen_command)(ast_node*)) {
    ast_node* node = MALLOC(sizeof(ast_node));
    node->type = type;
    node->gen_command = gen_command;
    node->tok = tok;
    node->lhs = NULL;
    node->rhs = NULL;
    return node;
}

void ast_tree_free(ast_node* node) {
    if (node == NULL)
        return;
    ast_tree_free(node->lhs);
    ast_tree_free(node->rhs);
    FREE(node);
}
