#ifndef _BYTECODE_H_
#define _BYTECODE_H_

typedef enum {
    ByteCodeAdd,
    ByteCodeSub,
    ByteCodeMul,
    ByteCodeDiv,
    ByteCodeMod,

    ByteCodeNegate,

    ByteCodeAssign,
    ByteCodeAddAssign,
    ByteCodeSubAssign,
    ByteCodeMulAssign,
    ByteCodeDivAssign,
    ByteCodeModAssign,

    ByteCodePushName,
    ByteCodePushConst,
    ByteCodeRefIden,
    ByteCodeAccessIden,
    ByteCodeInitVar,
    ByteCodeFuncDef,
    ByteCodeFuncEnd,
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

void gen_bytecode_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_add_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_sub_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_mul_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_div_assign(struct ast_node* node, struct vm* v);
void gen_bytecode_mod_assign(struct ast_node* node, struct vm* v);

void gen_bytecode_push_name(struct ast_node* node, struct vm* v);
void gen_bytecode_push_const(struct ast_node* node, struct vm* v);
void gen_bytecode_ref_iden(struct ast_node* node, struct vm* v);
void gen_bytecode_initvar(struct ast_node* node, struct vm* v);

void gen_bytecode_funcparam(struct ast_node* node, struct vm* v);
void gen_bytecode_funcdef(struct ast_node* node, struct vm* v);
void gen_bytecode_funcend(struct ast_node* node, struct vm* v);

#endif
