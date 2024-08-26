#include "trace_info.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

static void start_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "w");
    fclose(info->file);
    vector_reserve(info->ctx, 1 << 13);
    string_push(info->ctx, "{\"traceEvents\":[");
}
void end_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "a");

    if (!info->file) {
        free_string(info->ctx);
    	return;
    }
    fprintf(info->file, "%s]}", info->ctx);
    fclose(info->file);
    free_string(info->ctx);
}

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
    info->ctx_size = 0;
    info->ctx = make_string("");
	start_tracing(info);
}

static void fetch_trace_file(trace_info* info) {
    info->file = fopen(info->file_name, "a");
    fwrite(info->ctx, vector_size(info->ctx), 1, info->file);
    // fprintf(info->file, "%s", info->ctx);
    fclose(info->file);
}

void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
    char buf[128];
    sprintf(buf, "{\"name\":\"%s\",\"cat\":\"%s\",\"ph\":\"X\",\"ts\":%d,\"pid\":%d,\"tid\":%d,\"dur\":%d}", name, cat, ts, pid, tid, dur);
    if (info->count++ > 0) {
        string_push(info->ctx, ",");
        info->ctx_size++;
    }
    string_push(info->ctx, buf);
    info->ctx_size += (u32)strlen(buf) - 1;
    if (info->ctx_size >= 1 << 13) {
        fetch_trace_file(info);
        vector_status(info->ctx).size = 0;
        info->ctx_size = 0;
    }
}
