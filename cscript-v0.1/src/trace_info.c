#include "trace_info.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static void start_tracing(trace_info* info) {
    FILE* file = fopen(info->file_name, "w");
    fprintf(file, "{\"traceEvents\":[");
    fclose(file);
}
void end_tracing(trace_info* info) {
    FILE* file = fopen(info->file_name, "a");
    fprintf(file, "]}");
    fclose(file);
}

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
	start_tracing(info);
}

void submit_tracing_info(trace_info* info, const char* name, char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
    // char buf[128];
    // sprintf(buf, "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%d,\"pid\":%d,\"tid\":%d,\"dur\":%d}", name, cat, ts, pid, tid, dur == 0 ? 1 : dur);
    FILE* file = fopen(info->file_name, "a");
    if (!file) {
        return;
    }
    if (info->count++ > 0) {
        fprintf(file, "%s", ",");
    }
    // fprintf(file, "%s", buf);
    fprintf(file, "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%d,\"pid\":%d,\"tid\":%d,\"dur\":%d}", name, cat, ts, pid, tid, dur == 0 ? 1 : dur);
    fclose(file);
}

