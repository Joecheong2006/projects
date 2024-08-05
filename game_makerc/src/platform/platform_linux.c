#include "platform/platform.h"

#ifdef PLATFORM_LINUX
#define _POSIX_C_SOURCE 200809L
#include "core/assert.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

struct timespec begin;

i32 setup_platform(platform_state* target) {
	timespec_get(&begin, TIME_UTC);
	return 1;
}

void shutdown_platform(platform_state* target) {
}

f64 platform_get_time(void) {
	struct timespec end;
	timespec_get(&end, TIME_UTC);
	return end.tv_sec - begin.tv_sec + (end.tv_nsec - begin.tv_nsec) * 0.000000001;
}

void platform_sleep(i32 ms) {
	struct timespec ts; ts.tv_sec = ms * 0.0001;
	ts.tv_nsec = (ms % 1000) * 1000 * 1000;
	nanosleep(&ts, 0);
}

static char* console_color_map[] = {
	[ConsoleTextColorWhite] =  "37",
	[ConsoleTextColorBlack] = "30",
	[ConsoleTextColorRed] = "31",
	[ConsoleTextColorGreen] = "32",
	[ConsoleTextColorYellow] = "33",
	[ConsoleTextColorBlue] = "34",
	[ConsoleTextColorMagenta] = "34",
	[ConsoleTextColorCyan] = "36",
};

void platform_console_log(const char* msg, ConsoleTextColor color) {
	ASSERT_MSG(color <= ConsoleTextColorCyan && color >= ConsoleTextColorWhite, "invalid console text color");
	printf("\e[%sm%s\e[0m", console_color_map[color], msg);
}

#endif

