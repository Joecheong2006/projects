#ifndef _TRACING_H_
#define _TRACING_H_
#include "trace_info.h"

extern trace_info g_ti;

#define START_PROFILING()\
    BEGIN_SCOPE_PROFILING()

#define END_PROFILING(name)\
    END_SCOPE_PROFILING(g_ti, name)

#endif
