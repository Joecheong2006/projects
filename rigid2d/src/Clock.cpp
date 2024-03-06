#include "Clock.h"
#include "logger.h"

namespace mfw {
    Timer::Timer(const std::string& message)
        : message(std::move(message))
    {
        start = std::chrono::system_clock::now();
    }

    Timer::Timer()
        : message("")
    {
        start = std::chrono::system_clock::now();
    }

    Timer::~Timer() {
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<float> duration = end - start;
        LOG_INFO("[{}][{}ms]\n", message, duration.count() * 1000);
    }

}
