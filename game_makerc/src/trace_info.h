#ifndef _TRACE_INFO_
#define _TRACE_INFO_
#include "basic/util.h"
#include "stimer.h"

typedef struct {
    void* file;
    i32 pid, tid;
    u64 count;
} trace_info;

void setup_trace_info(trace_info* info);
void start_tracing(trace_info* info, char* file);
void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur);
void end_tracing(trace_info* info);

#define BEGIN_SCOPE_SESSION() {\
    stimer __timer;\
    start_stimer(&__timer);

#define END_SCOPE_SESSION(ti, name)\
    end_stimer(&__timer);\
    submit_tracing_info(&ti, name, "function", __timer.begin, ti.pid, ti.tid, 1000000 * __timer.dur);}

#endif
