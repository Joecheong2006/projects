#include "vm.h"
#include "tracing.h"

void init_vm(vm* v) {
    START_PROFILING();
    v->code = make_vector(u8);
    init_environment(&v->env);
    END_PROFILING(__func__);
}

void free_vm(vm* v) {
    START_PROFILING();
    free_environment(&v->env);
    free_vector(v->code);
    END_PROFILING(__func__);
}
