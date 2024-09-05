#include "platform/platform.h"

#ifdef PLATFORM_LINUX
#include "core/assert.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

static clock_t begin_t;

i32 setup_platform(platform_state* target) {
	target->os = PlatformLinux;
    begin_t = clock();
	return 1;
}

void shutdown_platform(platform_state* target) {
    (void)target;
}

f64 platform_get_time(void) {
    return (f64)(clock() - begin_t) / CLOCKS_PER_SEC;
}

void platform_sleep(i32 ms) {
    f64 factor = 1000.0 / CLOCKS_PER_SEC;
    f64 end = clock() * factor + ms;
    while (clock() * factor < end);
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
	printf("\x1B[%sm%s\x1B[0m", console_color_map[color], msg);
}

#endif

