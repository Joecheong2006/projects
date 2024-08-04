#ifndef _TRACE_INFO_H_
#define _TRACE_INFO_H_
#include "core/defines.h"
#include "stimer.h"

typedef struct {
    char* file_name;
    void* file;
    i32 pid, tid;
    u64 count;
} trace_info;

void setup_trace_info(trace_info* info);
void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur);
void end_tracing(trace_info* info);

#define BEGIN_SCOPE_SESSION() {\
    stimer __timer;\
    start_stimer(&__timer);

#define END_SCOPE_SESSION(ti, name)\
    end_stimer(&__timer);\
    submit_tracing_info(&ti, name, "function", 1000000 * __timer.begin, ti.pid, ti.tid, 1000000 * __timer.dur);}

#endif
