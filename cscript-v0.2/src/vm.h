#ifndef _VM_H_
#define _VM_H_
#include "environment.h"

struct vm {
    environment env;
    u8* code;
};

typedef struct vm vm;

void init_vm(vm* v);
void free_vm(vm* v);

#endif
