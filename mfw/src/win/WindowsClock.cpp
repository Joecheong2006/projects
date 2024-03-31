#include "win/WindowsClock.h"
#include "mfwlog.h"

#include <mmsystem.h>

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
        static const UINT min_period = []{
            TIMECAPS tc;
            timeGetDevCaps(&tc, sizeof(TIMECAPS));
            return tc.wPeriodMin;
        }();

        timeBeginPeriod(min_period);
        ::Sleep(millisecond);
        timeEndPeriod(min_period);
    }

}

