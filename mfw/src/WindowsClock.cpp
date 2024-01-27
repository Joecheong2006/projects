#include "WindowsClock.h"
#include <mfwlog.h>

namespace mfw {
    Time* Time::Instance = new WindowsTime();

    WindowsTime::WindowsTime() {
        LARGE_INTEGER ticks;
        if (!QueryPerformanceFrequency(&ticks))
            LOG_INFO("QueryPerformanceFrequency failed");
        pcfreq = ticks.QuadPart;
        QueryPerformanceCounter(&ticks);
        start = ticks.QuadPart;
    }

    f64 WindowsTime::GetCurrentImpl() {
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        return (ticks.QuadPart - start) / pcfreq;
    };

    void WindowsTime::SleepImpl(f32 millisecond) {
        ::Sleep(millisecond);
    }

}

