#include "platform/platform.h"

#ifdef PLATFORM_LINUX
#define _POSIX_C_SOURCE 200809L
#include "core/assert.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

i32 setup_platform(platform_state* target) {
}

void shutdown_platform(platform_state* target) {
}

f64 platform_get_time() {
	return (f64)clock() / CLOCKS_PER_SEC;
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	// return (f64)t.tv_sec + (f64)t.tv_nsec * 0.000000001;
}

void platform_sleep(i32 ms) {
#if _POSIX_C_SOURCE >= 199309L
	struct timespec ts; ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000 * 1000;
	nanosleep(&ts, 0);
#else
	if (ms >= 1000) {
		sleep(ms / 1000);
	}
	usleep((ms % 1000) * 1000);
#endif
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

