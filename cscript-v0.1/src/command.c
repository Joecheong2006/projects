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
    if (!cmd->arg2->exec(inter, cmd->arg2)) {
        return 0;
    }
    if (!cmd->arg1->exec(inter, cmd->arg1)) {
        return 0;
    }
    *cmd->data = primitive_data_add(cmd->arg1->data, cmd->arg2->data);
    return 1;
}

static i32 command_exec_minus(interpreter* inter, command* cmd) {
    if (!cmd->arg2->exec(inter, cmd->arg2)) {
        return 0;
    }
    if (!cmd->arg1->exec(inter, cmd->arg1)) {
        return 0;
    }
    *cmd->data = primitive_data_minus(cmd->arg1->data, cmd->arg2->data);
    return 1;
}

static i32 command_exec_multiply(interpreter* inter, command* cmd) {
    if (!cmd->arg2->exec(inter, cmd->arg2)) {
        return 0;
    }
    if (!cmd->arg1->exec(inter, cmd->arg1)) {
        return 0;
    }
    *cmd->data = primitive_data_multiply(cmd->arg1->data, cmd->arg2->data);
    return 1;
}

static i32 command_exec_divide(interpreter* inter, command* cmd) {
    if (!cmd->arg2->exec(inter, cmd->arg2)) {
        return 0;
    }
    if (!cmd->arg1->exec(inter, cmd->arg1)) {
        return 0;
    }
    *cmd->data = primitive_data_divide(cmd->arg1->data, cmd->arg2->data);
    return 1;
}

static i32 command_exec_negate(interpreter* inter, command* cmd) {
    if (!cmd->arg1->exec(inter, cmd->arg1)) {
    }
    *cmd->data = primitive_data_negate(cmd->arg1->data);
    return 1;
}

static i32 command_exec_vardecl(interpreter* inter, command* cmd) {
    // TODO: create variable from cmd->arg2
    if (!cmd->arg2->exec(inter, cmd->arg2)) {
        return 0;
    }
    return 1;
}

void command_free(command* cmd) {
    if (cmd == NULL)
        return;
    command_free(cmd->arg1);
    command_free(cmd->arg2);
    FREE(cmd);
}

command* make_command(i32(*exec)(interpreter*,command*), primitive_data* data) {
    command* cmd = MALLOC(sizeof(command));
    cmd->arg1 = cmd->arg2 = NULL;
    cmd->data = data;
    cmd->exec = exec;
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
    result->arg1 = node->lhs->gen_command(node->lhs);
    result->arg2 = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_minus(ast_node* node) {
    command* result = make_command(command_exec_minus, &node->tok->val);
    result->arg1 = node->lhs->gen_command(node->lhs);
    result->arg2 = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_multiply(ast_node* node) {
    command* result = make_command(command_exec_multiply, &node->tok->val);
    result->arg1 = node->lhs->gen_command(node->lhs);
    result->arg2 = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_divide(ast_node* node) {
    command* result = make_command(command_exec_divide, &node->tok->val);
    result->arg1 = node->lhs->gen_command(node->lhs);
    result->arg2 = node->rhs->gen_command(node->rhs);
    return result;
}

command* gen_command_negate(ast_node* node) {
    command* result = make_command(command_exec_negate, &node->tok->val);
    result->arg1 = node->lhs->gen_command(node->lhs);
    return result;
}

command* gen_command_identifier(ast_node* node) {
    command* result = make_command(command_exec_negate, &node->tok->val);
    result->name = node->tok->val.string;
    result->data->string = 0;
    return result;
}

command* gen_command_vardecl(ast_node* node) {
    command* result = make_command(command_exec_vardecl, &node->tok->val);
    result->arg1 = node->lhs->gen_command(node->lhs);
    result->arg2 = node->rhs->gen_command(node->rhs);
    return result;
}
