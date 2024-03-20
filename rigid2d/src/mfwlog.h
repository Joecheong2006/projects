#pragma once

#include "logger.h"
#if defined(DEBUG) || defined(_DEBUG)
#include <mfwpch.h>

#define ASSERT(x) \
    if(!(x)) __debugbreak();
#define LOG_TRACE(...) \
    Log::Trace(__VA_ARGS__);
#define LOG_DEBUG(...) \
    Log::Debug(__VA_ARGS__);
#define LOG_INFO(...) \
    Log::Info(__VA_ARGS__);
#define LOG_WARN(...) \
    Log::Warn(__VA_ARGS__);
#define LOG_ERROR(...) \
    Log::Error(__VA_ARGS__);
#define LOG_FATAL(...) \
    Log::Fatal(__VA_ARGS__);

namespace Log {
    template <typename T>
    struct Pattern<std::vector<T>> {
        static void Log(const std::vector<T>& value, const std::string& format) {
            (void)format;
            Pattern<char>::Log('[', "");
            for (size_t i = 0; i < value.size() - 1; ++i) {
                Pattern<T>::Log(value[i], format);
                Pattern<std::string>::Log(", ", "");
            }
            Pattern<T>::Log(value.back(), format);
            Pattern<char>::Log(']', "");
        }
    };

}

#else
#define ASSERT(x)
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)
#endif

