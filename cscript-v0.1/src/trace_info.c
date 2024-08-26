#include "trace_info.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static void start_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "w");
    fclose(info->file);
    string_push(info->ctx, "{\"traceEvents\":[");
}
void end_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "a");

    if (!info->file) {
        free_string(info->ctx);
    	return;
    }
    string_push(info->ctx, "]}");
    fprintf(info->file, "%s", info->ctx);
    fclose(info->file);
    free_string(info->ctx);
}

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
    info->ctx = make_string("");
	start_tracing(info);
}

void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
    char buf[128];
    sprintf(buf, "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%d,\"pid\":%d,\"tid\":%d,\"dur\":%d}", name, cat, ts, pid, tid, dur);
    if (info->count++ > 0) {
        string_push(info->ctx, ",");
    }
    string_push(info->ctx, buf);
}
