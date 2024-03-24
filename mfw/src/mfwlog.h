#pragma once

#include "logger.h"
#if defined(DEBUG) || defined(_DEBUG)
#define ASSERT(x) if(!(x)) __debugbreak();
#define LOG_TRACE(...) Log::Trace(__VA_ARGS__);
#define LOG_DEBUG(...) Log::Debug(__VA_ARGS__);
#define LOG_INFO(...) Log::Info(__VA_ARGS__);
#define LOG_WARN(...) Log::Warn(__VA_ARGS__);
#define LOG_ERROR(...) Log::Error(__VA_ARGS__);
#define LOG_FATAL(...) Log::Fatal(__VA_ARGS__);

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
#include "map.h"
#define TOVOID(x) (void)(x);
#define ASSERT(x) TOVOID(x)
#define LOG_TRACE(...) MAP(TOVOID, __VA_ARGS__)
#define LOG_DEBUG(...) MAP(TOVOID, __VA_ARGS__)
#define LOG_INFO(...) MAP(TOVOID, __VA_ARGS__)
#define LOG_WARN(...) MAP(TOVOID, __VA_ARGS__)
#define LOG_ERROR(...) MAP(TOVOID, __VA_ARGS__)
#define LOG_FATAL(...) MAP(TOVOID, __VA_ARGS__)
#endif

