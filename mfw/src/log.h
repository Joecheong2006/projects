#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#include "mfwpch.h"
namespace mfw {
    template <typename... Args>
    void print(const Args&... args) {
        (std::cout << ... << args);
    }

    template <typename... Args>
    void println(const Args&... args) {
        (std::cout << ... << args) << '\n';
    }

}

#define ASSERT(x) \
    if(!(x)) __debugbreak();
#define LOG_INFO(...) \
    mfw::print(__VA_ARGS__);
#define LOG_INFOLN(...) \
    mfw::println(__VA_ARGS__);
#else
#define ASSERT(x)
#define MFW_LOG_INFO(format, ...)
#define MFW_LOG_INFOLN(format, ...)
#endif

