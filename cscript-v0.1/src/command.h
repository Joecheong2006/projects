#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "primitive_data.h"
#include "interpreter.h"

typedef struct command command;
struct command {
    command *arg1, *arg2;
    primitive_data* data;
    const char* name;
    i32(*exec)(interpreter*,command*);
};

command* make_command(i32(*exec)(interpreter*,command*), primitive_data* data);
void command_free(command* cmd);

struct ast_node;
command* gen_command_null(struct ast_node* node);
command* gen_command_ret(struct ast_node* node);
command* gen_command_add(struct ast_node* node);
command* gen_command_minus(struct ast_node* node);
command* gen_command_multiply(struct ast_node* node);
command* gen_command_divide(struct ast_node* node);
command* gen_command_negate(struct ast_node* node);
command* gen_command_identifier(struct ast_node* node);
command* gen_command_vardecl(struct ast_node* node);

#endif
