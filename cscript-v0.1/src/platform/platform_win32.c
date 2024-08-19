#include "platform/platform.h"

#ifdef PLATFORM_WINDOWS

#include "core/assert.h"
#include "core/log.h"

#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

typedef struct {
	HINSTANCE h_instance;
	HWND hwnd;
} win_state;

static f64 pcfreq;
static i64 start;
static UINT min_period;

i32 setup_platform(platform_state* target) {
	ASSERT_MSG(target, "invalid platform target to setup");
	target->state = 0;
	target->os = PlatformWindows;
	win_state* state = malloc(sizeof(win_state));
	if (!state) {
		return 0;
	}
	state->h_instance = GetModuleHandle(0);
	{
	    LARGE_INTEGER ticks;
	    if (!QueryPerformanceFrequency(&ticks)) {
	        LOG_INFO("%s\n", "QueryPerformanceFrequency failed");
	    }
	    pcfreq = ticks.QuadPart;
	    QueryPerformanceCounter(&ticks);
	    start = ticks.QuadPart;
	}
    {
        TIMECAPS tc;
        timeGetDevCaps(&tc, sizeof(TIMECAPS));
        min_period = tc.wPeriodMin;
    }

    target->state = state;
    return 1;
}


void shutdown_platform(platform_state* target) {
	ASSERT_MSG(target, "invalid platform target");
	ASSERT_MSG(target->state, "uninitialize platform target");
	free(target->state);
}

f64 platform_get_time(void) {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return (f64)(ticks.QuadPart - start) / pcfreq;
}

void platform_sleep(i32 ms) {
	timeBeginPeriod(min_period);
	Sleep(ms);
	timeEndPeriod(min_period);
}

static i32 console_color_map[] = {
	[ConsoleTextColorWhite] =  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	[ConsoleTextColorBlack] = 0,
	[ConsoleTextColorRed] = FOREGROUND_RED,
	[ConsoleTextColorGreen] = FOREGROUND_GREEN,
	[ConsoleTextColorYellow] = FOREGROUND_RED | FOREGROUND_GREEN,
	[ConsoleTextColorBlue] = FOREGROUND_BLUE,
	[ConsoleTextColorMagenta] = FOREGROUND_RED | FOREGROUND_BLUE,
	[ConsoleTextColorCyan] = FOREGROUND_GREEN | FOREGROUND_BLUE,
};

void platform_console_log(const char* msg, ConsoleTextColor color) {
	ASSERT_MSG(color <= ConsoleTextColorCyan && color >= ConsoleTextColorWhite, "invalid console text color");
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, console_color_map[color]);
#if defined(_MSC_VER)
    OutputDebugString(msg);
#else
    WriteConsole(handle, msg, strlen(msg), NULL, NULL);
#endif
    SetConsoleTextAttribute(handle, console_color_map[ConsoleTextColorWhite]);
}

#endif

