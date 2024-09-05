#include "tracing.h"

trace_info g_ti = { .file_name = "tracing.json" };

__attribute__((constructor(110)))
static void setup_global_trace_info(void) {
    setup_trace_info(&g_ti);
}

__attribute__((destructor(110)))
static void end_global_trace_info(void) {
    end_tracing(&g_ti);
}

