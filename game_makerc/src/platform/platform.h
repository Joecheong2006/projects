#ifndef _PLATFORM_PLATFORM_H_
#define _PLATFORM_PLATFORM_H_
#include "core/defines.h"

typedef enum {
	PlatformUnkown,
	PlatformWindows,
	PlatformLinux,
} PlatformOs;

typedef struct {
	void* state;
	PlatformOs os;
} platform_state;

i32 setup_platform(platform_state* target);
void shutdown_platform(platform_state* target);

f64 platform_get_time(void);
void platform_sleep(i32 ms);

typedef enum {
    ConsoleTextColorWhite = 0,
    ConsoleTextColorBlack,
    ConsoleTextColorRed,
    ConsoleTextColorGreen,
    ConsoleTextColorYellow,
    ConsoleTextColorBlue,
    ConsoleTextColorMagenta,
    ConsoleTextColorCyan,
} ConsoleTextColor;

void platform_console_log(const char* msg, ConsoleTextColor color);

#endif
