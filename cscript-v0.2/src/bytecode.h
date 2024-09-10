#ifndef _BYTECODE_H_
#define _BYTECODE_H_

typedef enum {
    ByteCodeAdd,
    ByteCodeSub,
    ByteCodeMul,
    ByteCodeDiv,
    ByteCodeMod,

    ByteCodeNegate,

    ByteCodeEqual,
    ByteCodeNotEqual,
    ByteCodeGreaterThan,
    ByteCodeLessThan,
    ByteCodeGreaterThanEqual,
    ByteCodeLessThanEqual,

    ByteCodeAssign,
    ByteCodeAddAssign,
    ByteCodeSubAssign,
    ByteCodeMulAssign,
    ByteCodeDivAssign,
    ByteCodeModAssign,

    ByteCodePop,
    ByteCodePushName,
    ByteCodePushConst,
    ByteCodePushNull,
    ByteCodePushTrue,
    ByteCodePushFalse,
    ByteCodeRefIden,
    ByteCodeAccessIden,
    ByteCodeInitVar,
    ByteCodeFuncDef,
    ByteCodeFuncEnd,
    ByteCodeFuncall,
    ByteCodeReturn,
    ByteCodeReturnNone,
} ByteCode;

struct ast_node;
struct vm;
void gen_bytecode_bracket(struct ast_node* node, struct vm* v);
void gen_bytecode_add(struct ast_node* node, struct vm* v);
void gen_bytecode_sub(struct ast_node* node, struct vm* v);
void gen_bytecode_mul(struct ast_node* node, struct vm* v);
void gen_bytecode_div(struct ast_node* node, struct vm* v);
void gen_bytecode_mod(struct ast_node* node, struct vm* v);

void gen_bytecode_negate(struct ast_node* node, struct vm* v);

void gen_bytecode_equal(struct ast_node* node, struct vm* v);
void gen_bytecode_not_equal(struct ast_node* node, struct vm* v);
void gen_bytecode_greater_than(struct ast_node* node, struct vm* v);
void gen_bytecode_less_than(struct ast_node* node, struct vm* v);
void gen_bytecode_greater_than_equal(struct ast_node* node, struct vm* v);
void gen_bytecode_less_than_equal(struct ast_node* node, struct vm* v);

void gen_bytecode_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_add_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_sub_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_mul_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_div_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_mod_assign(struct ast_node* node, struct vm* v);

void gen_bytecode_pop(struct ast_node* node, struct vm* v);
void gen_bytecode_push_name(struct ast_node* node, struct vm* v);
void gen_bytecode_push_const(struct ast_node* node, struct vm* v);
void gen_bytecode_push_null(struct ast_node* node, struct vm* v);
void gen_bytecode_push_true(struct ast_node* node, struct vm* v);
void gen_bytecode_push_false(struct ast_node* node, struct vm* v);
void gen_bytecode_ref_iden(struct ast_node* node, struct vm* v);
void gen_bytecode_access_iden(struct ast_node* node, struct vm* v);
void gen_bytecode_initvar(struct ast_node* node, struct vm* v);

void gen_bytecode_funcparam(struct ast_node* node, struct vm* v);
void gen_bytecode_funcdef(struct ast_node* node, struct vm* v);
void gen_bytecode_funcend(struct ast_node* node, struct vm* v);
void gen_bytecode_arguments(struct ast_node* node, struct vm* v);
void gen_bytecode_funcall(struct ast_node* node, struct vm* v);
void gen_bytecode_return(struct ast_node* node, struct vm* v);

#endif
