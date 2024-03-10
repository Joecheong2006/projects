#pragma once
#include <mfwpch.h>
#include "util.h"

namespace mfw {
    class Time {
        friend class Application;
    public:
        virtual ~Time() {}
        inline static f64 GetCurrent() { return Instance->GetCurrentImpl(); }
        inline static void Sleep(f32 millisecond) { Instance->SleepImpl(millisecond); }

    private:
        virtual f64 GetCurrentImpl() = 0;
        virtual void SleepImpl(f32 millisecond) = 0;

        static Time* Instance;

    };

    class Timer {
    public:
        Timer(const std::string& message);
        Timer();
        ~Timer();

    private:
        const std::string message;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<i64, std::ratio<1, 1000000000>>> start;

    };
}

#define START_CLOCK_TIMER(message) mfw::Timer __timer(message)

