#include "trace_info.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

void setup_trace_info(trace_info* info) {
	info->pid = getpid();
	// info->tid = gettid();
	info->tid = 0;
	info->count = 0;
}

void start_tracing(trace_info* info, char* file) {
    info->file = fopen(file, "w+");
    fprintf(info->file, "{\n\t\"traceEvents\":[\n");
}

void submit_tracing_info(trace_info* info, const char name[], char* cat, i32 ts, i32 pid, i32 tid, i32 dur) {
	if (info->count++ > 0) {
	    fprintf(info->file, ",\n");
	}
    fprintf(info->file, "\t\t{\n");
    fprintf(info->file, "\t\t\t\"name\":\"%s\",\n", name);
    fprintf(info->file, "\t\t\t\"cat\":\"%s\",\n", cat);
    fprintf(info->file, "\t\t\t\"ph\":\"%s\",\n", "X");
    fprintf(info->file, "\t\t\t\"ts\":%d,\n", ts);
    fprintf(info->file, "\t\t\t\"pid\":%d,\n", pid);
    fprintf(info->file, "\t\t\t\"tid\":%d,\n", tid);
    fprintf(info->file, "\t\t\t\"dur\":%d\n", dur);
    fprintf(info->file, "\t\t}");
}

void end_tracing(trace_info* info) {
    fprintf(info->file, "\r\t]\n}\n");
    fclose(info->file);
}
