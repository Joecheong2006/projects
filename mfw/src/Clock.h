#pragma once
#include <chrono>
#include <mfwpch.h>
#include "util.h"

namespace mfw {
    namespace Clock {
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
}

#define START_CLOCK_DURATION(message) mfw::Clock::Timer __timer(message)

