#include "trace_info.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static void start_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "w");
    if (!info->file) {
    	return;
    }
    fprintf(info->file, "{\n\t\"traceEvents\":[\n");

    fclose(info->file);
}
void end_tracing(trace_info* info) {
    info->file = fopen(info->file_name, "a");

    if (!info->file) {
    	return;
    }
    fprintf(info->file, "\t]\n}\n");
    fclose(info->file);
}

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
	start_tracing(info);
}

void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
    char buf[2560];
    sprintf(buf, "\t\t{\n\t\t\t\"name\":\"%s\",\n\t\t\t\"cat\":\"%s\",\n\t\t\t\"ph\":\"X\",\n\t\t\t\"ts\":%d,\n\t\t\t\"pid\":%d,\n\t\t\t\"tid\":%d,\n\t\t\t\"dur\":%d\n\t\t}", name, cat, ts, pid, tid, dur);

    info->file = fopen(info->file_name, "a");
    if (!info->file) {
    	return;
    }
	if (info->count++ > 0) {
	    fprintf(info->file, ",\n");
	}
    fprintf(info->file, "%s", buf);
    fclose(info->file);
}
