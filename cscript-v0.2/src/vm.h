#ifndef _VM_H_
#define _VM_H_
#include "environment.h"

struct vm {
    environment env;
    u64 ip;
    u8* code;
};

typedef struct vm vm;

void init_vm(vm* v);
void free_vm(vm* v);

struct ast_node;
void vm_gen_bytecode(vm* v, struct ast_node**);
error_info vm_run(vm* v);

#endif
