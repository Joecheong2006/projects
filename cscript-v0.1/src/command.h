#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "primitive_data.h"
#include "interpreter.h"

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
    void*(*exec)(interpreter*,command*);
    void(*destroy)(command*);
    CommandType type;
};

command* make_command(CommandType type, u64 type_size, void*(*exec)(interpreter*,command*), void(*destroy)(command*));
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

struct ast_node;
command* gen_command_get_constant(struct ast_node* node);
command* gen_command_add(struct ast_node* node);
command* gen_command_minus(struct ast_node* node);
command* gen_command_multiply(struct ast_node* node);
command* gen_command_divide(struct ast_node* node);
command* gen_command_negate(struct ast_node* node);
command* gen_command_vardecl(struct ast_node* node);

#endif
