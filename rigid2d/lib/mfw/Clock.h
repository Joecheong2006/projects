#include "mfwpch.h"
#pragma once

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
        Timer();
        ~Timer();

        f32 getDuration();
        void log();

    private:
        f64 duration, start;

    };
    
    class Clock {
    private:
        const std::string message;
        Timer timer;

    public:
        Clock(const std::string& message);
        ~Clock();

    };
}

#define START_CLOCK_TIMER(message) mfw::Clock __duration(message)

