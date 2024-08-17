#include "command.h"
#include "container/memallocate.h"
#include "parser.h"

static i32 command_exec_null(interpreter* inter, command* cmd) {
    (void)inter, (void)cmd;
    return 1;
}

static i32 command_exec_ret(interpreter* inter, command* cmd) {
    (void)inter, (void)cmd;
    return 1;
}

static i32 command_exec_add(interpreter* inter, command* cmd) {
    cmd->rhs->exec(inter, cmd->rhs);
    cmd->lhs->exec(inter, cmd->lhs);
    *cmd->data = primitive_data_add(cmd->lhs->data, cmd->rhs->data);
    return 1;
}

static i32 command_exec_minus(interpreter* inter, command* cmd) {
    cmd->rhs->exec(inter, cmd->rhs);
    cmd->lhs->exec(inter, cmd->lhs);
    *cmd->data = primitive_data_minus(cmd->lhs->data, cmd->rhs->data);
    return 1;
}

static i32 command_exec_multiply(interpreter* inter, command* cmd) {
    cmd->rhs->exec(inter, cmd->rhs);
    cmd->lhs->exec(inter, cmd->lhs);
    *cmd->data = primitive_data_multiply(cmd->lhs->data, cmd->rhs->data);
    return 1;
}

static i32 command_exec_divide(interpreter* inter, command* cmd) {
    cmd->rhs->exec(inter, cmd->rhs);
    cmd->lhs->exec(inter, cmd->lhs);
    *cmd->data = primitive_data_divide(cmd->lhs->data, cmd->rhs->data);
    return 1;
}

static i32 command_exec_negate(interpreter* inter, command* cmd) {
    cmd->lhs->exec(inter, cmd->lhs);
    *cmd->data = primitive_data_negate(cmd->lhs->data);
    return 1;
}

static i32 command_exec_vardecl(interpreter* inter, command* cmd) {
    cmd->rhs->exec(inter, cmd->rhs);
    cmd->data = cmd->rhs->data;
    return 1;
}

void command_free(command* cmd) {
    if (cmd == NULL)
        return;
    command_free(cmd->lhs);
    command_free(cmd->rhs);
    FREE(cmd);
}

command* make_command(i32(*exec)(interpreter*,command*), primitive_data* data) {
    command* cmd = MALLOC(sizeof(command));
    cmd->data = data;
    cmd->exec = exec;
    cmd->lhs = cmd->rhs = NULL;
    return cmd;
}

command* gen_command_null(ast_node* node) {
    return make_command(command_exec_null, &node->tok->val);
}

command* gen_command_ret(ast_node* node) {
    return make_command(command_exec_ret, &node->tok->val);
}

command* gen_command_add(ast_node* node) {
    command* result = make_command(command_exec_add, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    result->rhs = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_minus(ast_node* node) {
    command* result = make_command(command_exec_minus, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    result->rhs = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_multiply(ast_node* node) {
    command* result = make_command(command_exec_multiply, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    result->rhs = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_divide(ast_node* node) {
    command* result = make_command(command_exec_divide, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    result->rhs = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_negate(ast_node* node) {
    command* result = make_command(command_exec_negate, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    return result;
}

command* gen_command_vardecl(ast_node* node) {
    command* result = make_command(command_exec_vardecl, &node->tok->val);
    result->lhs = node->lhs->gen_command(node->lhs);
    result->rhs = node->rhs->gen_command(node->rhs);
    return result;
}
