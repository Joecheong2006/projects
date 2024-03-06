#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#include <mfwpch.h>
#include "logger.h"

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
#define MFW_LOG_INFO(format, ...)
#define MFW_LOG_INFOLN(format, ...)
#endif

