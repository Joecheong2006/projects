#ifndef _BYTECODE_H_
#define _BYTECODE_H_

typedef enum {
    ByteCodePushRef,
    ByteCodePushConst,
    ByteCodeAccessRef,
    ByteCodeAdd,
    ByteCodeSub,
    ByteCodeMul,
    ByteCodeDiv,
    ByteCodeMod,
    ByteCodeNegate,
    ByteCodeInitVar,
} ByteCode;

struct ast_node;
struct vm;
void gen_bytecode_push_ref(struct ast_node* node, struct vm*);
void gen_bytecode_push(struct ast_node* node, struct vm*);
void gen_bytecode_add(struct ast_node* node, struct vm*);
void gen_bytecode_minus(struct ast_node* node, struct vm*);
void gen_bytecode_multiply(struct ast_node* node, struct vm*);
void gen_bytecode_divide(struct ast_node* node, struct vm*);
void gen_bytecode_modulus(struct ast_node* node, struct vm*);
void gen_bytecode_negate(struct ast_node* node, struct vm*);
void gen_bytecode_initvar(struct ast_node* node, struct vm*);

#endif
