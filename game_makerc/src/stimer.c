#include "stimer.h"
#include <time.h>
#include <stdio.h>

#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
static f32 pcfreq;
static i64 start;
__attribute__((constructor(130)))
static void init_windows_time() {
    LARGE_INTEGER ticks;
    if (!QueryPerformanceFrequency(&ticks)) {
        printf("QueryPerformanceFrequency failed\n");
    }
    pcfreq = ticks.QuadPart;
    QueryPerformanceCounter(&ticks);
    start = ticks.QuadPart;
}

f32 get_current_time() {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return (ticks.QuadPart - start) / pcfreq;
}
#else
f32 get_current_time() {
    return (f32)clock() / CLOCKS_PER_SEC;
}
#endif

void start_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    // t->begin = (f64)clock() / CLOCKS_PER_SEC;
    t->begin = get_current_time();
}

void end_stimer(stimer* t) {
    ASSERT_MSG(t != NULL, "invalid stimer");
    t->dur = (f64)(clock() - t->begin) / CLOCKS_PER_SEC;
}
