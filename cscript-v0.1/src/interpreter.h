#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include "container/vector.h"
#include "primitive_data.h"
#include "error_info.h"
#include "environment.h"

typedef enum {
    CommandTypeVarDecl,

    CommandTypeAssignment,
    CommandTypeBinaryOperation,
    CommandTypeNegateOperation,
    CommandTypeAccess,
    CommandTypeGetConstant,
} CommandType;

typedef struct command command;
struct command {
    void(*destroy)(command*);
    CommandType type;
};

command* make_command(CommandType type, u64 type_size, void(*destroy)(command*));
void* get_command_true_type(const command* cmd);

typedef struct {
    i32 pointer;
    environment env;
    vector(command*) ins;
} interpreter;

void init_interpreter(interpreter* inter, vector(command*) ins);
void free_interpreter(interpreter* inter);
error_info interpret_command(interpreter* inter);

typedef struct {
    command *lhs, *rhs;
    error_info(*cal)(interpreter*, primitive_data*, command*);
} command_binary_operation;

typedef struct {
    primitive_data data;
} command_get_constant;

typedef struct {
    command* data;
} command_negate_operation;

typedef struct command_vardecl command_vardecl;
struct command_vardecl {
    const char* variable_name;
    command* expr;
    i32 atrribute, line_on_exec;
};

typedef struct {
    const char* name;
    command* access;
} command_access;

typedef struct {
    command* mem;
    command* expr;
    i32 line_on_exec;
    error_info(*exec)(interpreter*, const command*);
} command_assign;

struct ast_node;
command* make_command_get_constant(struct ast_node* node);
command* make_command_access(struct ast_node* node);
command* make_command_add(struct ast_node* node);
command* make_command_minus(struct ast_node* node);
command* make_command_multiply(struct ast_node* node);
command* make_command_divide(struct ast_node* node);
command* make_command_modulus(struct ast_node* node);
command* make_command_negate(struct ast_node* node);
command* make_command_vardecl(struct ast_node* node);
command* make_command_assignment(struct ast_node* node);
command* make_command_add_assign(struct ast_node* node);
command* make_command_minus_assign(struct ast_node* node);
command* make_command_multiply_assign(struct ast_node* node);
command* make_command_divide_assign(struct ast_node* node);
command* make_command_modulus_assign(struct ast_node* node);

#endif
