#include "trace_info.h"
#include "container/string.h"
#include "container/vector.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#define CTX_SIZE 1 << 13

static void start_tracing(trace_info* info) {
    FILE* file = fopen(info->file_name, "w");
    fprintf(file, "{\"traceEvents\":[");
    fclose(file);
    info->ctx = make_string("");
    vector_reserve(info->ctx, CTX_SIZE);
}
void end_tracing(trace_info* info) {
    FILE* file = fopen(info->file_name, "a");
    fprintf(file, "]}");
    fclose(file);
    free_string(info->ctx);
}

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
	start_tracing(info);
}

void submit_tracing_info(trace_info* info, const char* name, char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
    char buf[128];
    sprintf(buf, "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%d,\"pid\":%d,\"tid\":%d,\"dur\":%d}", name, cat, ts, pid, tid, dur == 0 ? 1 : dur);
    if (vector_size(info->ctx) + strlen(buf) >= CTX_SIZE) {
        FILE* file = fopen(info->file_name, "a");
        fprintf(file, "%s", info->ctx);
        fclose(file);
        vector_size(info->ctx) = 0;
    }
    if (info->count++ > 0) {
        string_push(info->ctx, ",");
    }
    string_push(info->ctx, buf);
}

