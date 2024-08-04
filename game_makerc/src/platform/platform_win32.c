#include "platform/platform.h"

#if PLATFORM_WINDOWS

#include "core/defines.h"

#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

typedef struct {
	HINSTANCE h_instance;
	f64 pcfreq;
	i64 start;
	UINT min_period;
} win_state;

i32 setup_platform(platform_state* target) {
	ASSERT_MSG(target, "invalid platform target to setup");
	target->state = 0;
	win_state* state = melloc(sizeof(win_state));
	if (!state) {
		return 0;
	}
	state->h_instance = GetModuleHandle(0);
	{
	    LARGE_INTEGER ticks;
	    if (!QueryPerformanceFrequency(&ticks))
	        LOG_INFO("QueryPerformanceFrequency failed");
	    state->pcfreq = ticks.QuadPart;
	    QueryPerformanceCounter(&ticks);
	    state->start = ticks.QuadPart;
	}
    {
        TIMECAPS tc;
        timeGetDevCaps(&tc, sizeof(TIMECAPS));
        state->min_period = tc.wPeriodMin;
    }

    target->state = state;
    return 1;
}


void shutdown_platform(platform_state* target) {
	ASSERT_MSG(target, "invalid platform target");
	ASSERT_MSG(target->state, "uninitialize platform target");
	free(target->state);
}

f64 platform_get_time() {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return (ticks.QuadPart - start) / pcfreq;
}

void platform_sleep(i32 ms) {
	timeBeginPeriod(min_period);
	Sleep(ms);
	timeEndPeriod(min_period);
}

void platform_console_log(const char* msg, i32 color) {
}

#endif

