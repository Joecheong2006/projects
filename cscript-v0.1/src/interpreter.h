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
    CommandTypeArgument,
    CommandTypeReference,
    CommandTypeReferenceIdentifier,
    CommandTypeFuncParam,
    CommandTypeFuncDef,
    CommandTypeFuncall,
    CommandTypeReturn,
    CommandTypeGetConstant,
} CommandType;

typedef struct command command;
struct command {
    void(*destroy)(command*);
    CommandType type;
    i32 line_on_exec;
};

command* make_command(CommandType type, u32 line_on_exec, u64 type_size, void(*destroy)(command*));
const void* get_command_true_type(const command* cmd);

typedef struct {
    i32 pointer;
    environment env;
    vector(command*) ins;
} interpreter;

void init_interpreter(interpreter* inter, vector(command*) ins);
void free_interpreter(interpreter* inter);
error_info exec_command(interpreter* inter, command* cmd);
error_info interpret_command(interpreter* inter);

typedef struct {
    command *lhs, *rhs;
    error_info(*cal)(interpreter*, const command*, primitive_data*);
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
    i32 atrribute;
};

typedef struct {
    const char* name;
} command_access_identifier;

typedef struct {
    vector(command*) args;
} command_argument;

typedef struct {
    vector(const char*) params;
} command_funcparam;

typedef struct {
    const char* identifier;
    command* params;
    vector(command*) ins;
} command_funcdef;

typedef struct {
    const char* name;
    command* args;
} command_funcall;

typedef struct {
    command* expr;
} command_return;

typedef struct {
    command* id;
    command* next_ref;
    error_info(*reference)(interpreter*, command*, object_carrier**);
} command_reference;

typedef struct {
    command* mem;
    command* expr;
    error_info(*exec)(interpreter*, const command*);
} command_assign;

struct ast_node;
command* make_command_get_constant(struct ast_node* node);
command* make_command_argument(struct ast_node* node);
command* make_command_funcparam(struct ast_node* node);
command* make_command_funcdef(struct ast_node* node);
command* make_command_funcall(struct ast_node* node);
command* make_command_return(struct ast_node* node);
command* make_command_reference_identifier(struct ast_node* node);
command* make_command_reference_funcall(struct ast_node* node);
command* make_command_access_identifier(struct ast_node* node);

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

command* make_command_cmp_equal(struct ast_node* node);
command* make_command_cmp_not_equal(struct ast_node* node);
command* make_command_cmp_greater_than(struct ast_node* node);
command* make_command_cmp_less_than(struct ast_node* node);
command* make_command_cmp_greater_than_equal(struct ast_node* node);
command* make_command_cmp_less_than_equal(struct ast_node* node);

#endif
