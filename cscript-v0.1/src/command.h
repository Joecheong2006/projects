#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "primitive_data.h"

typedef enum {
    CommandTypeBinaryOperation,
    CommandTypeNegateOperation,
    CommandTypeAccessVariable,
    CommandTypeGetConstant,
    CommandTypeVarDecl,
    CommandTypeNone,
} CommandType;

typedef struct command command;
struct command {
    void*(*exec)(command*);
    void(*destroy)(command*);
    CommandType type;
    i32 line_on_exec;
};

command* make_command(CommandType type, u64 type_size, i32 line, void*(*exec)(command*), void(*destroy)(command*));
void* get_command_true_type(command* cmd);

typedef struct {
    command *lhs, *rhs;
    primitive_data(*cal)(command*);
} command_binary_operation;

typedef struct {
    primitive_data data;
} command_get_constant;

typedef struct {
    command* data;
} command_negate_operation;

typedef struct {
    const char* variable_name;
    command* expr;
    i32 atrribute;
} command_vardecl;

typedef struct {
    const char* variable_name;
    const char* access_name;
} command_access;

struct ast_node;
command* make_command_get_constant(struct ast_node* node);
command* make_command_add(struct ast_node* node);
command* make_command_minus(struct ast_node* node);
command* make_command_multiply(struct ast_node* node);
command* make_command_divide(struct ast_node* node);
command* make_command_modulus(struct ast_node* node);
command* make_command_negate(struct ast_node* node);
command* make_command_vardecl(struct ast_node* node);

#endif
