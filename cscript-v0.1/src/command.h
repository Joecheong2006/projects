#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "primitive_data.h"

// TODO(Aug17): put this somewhere else.
typedef struct {
    i32 pointer;
} interpreter;

typedef struct command command;
struct command {
    command *lhs, *rhs;
    primitive_data* data;
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
command* gen_command_vardecl(struct ast_node* node);

#endif
