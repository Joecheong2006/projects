#pragma once

#include <stdio.h>
#include <string>

namespace Log {
    template <typename T>
    struct Pattern {
        explicit Pattern() {}
        static void Log(const T& value) { (void)value; }
    };
}

#define DEFINE_PATTERN_BASIC_LOG(type, pattern, ...)\
        template <>\
        struct Log::Pattern<type> {\
            static void Log(const type& value) {\
                printf(pattern, __VA_ARGS__);\
            }\
        };

#define DEFINE_PATTERN_BASIC_LOGLN(type, pattern, ...)\
    DEFINE_PATTERN_BASIC_LOG(type, pattern "\n", __VA_ARGS__);\

DEFINE_PATTERN_BASIC_LOG(int, "%d", value);
DEFINE_PATTERN_BASIC_LOG(float, "%g", value);
DEFINE_PATTERN_BASIC_LOG(double, "%lg", value);
DEFINE_PATTERN_BASIC_LOG(char*, "%s", value);
DEFINE_PATTERN_BASIC_LOG(char, "%c", value);
DEFINE_PATTERN_BASIC_LOG(std::string, "%s", value.c_str());

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
namespace Log {
    enum class ColorPattern {
        Reset = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        Black = 0,
        Red = FOREGROUND_RED,
        Green = FOREGROUND_GREEN,
        Yellow = Green | Red,
        Blue = FOREGROUND_BLUE,
        Magenta = Blue | Red,
        Cyan = Blue | Green,
        White = Reset,
    };
    void SetColor(int colorPattern);
    void SetColor(ColorPattern colorPattern = ColorPattern::Reset);
}
#else
#include <stdlib.h>
namespace Log {
    enum class ColorPattern {
        Reset = 0,
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
    };

    void SetColor(int colorPattern);
    void SetColor(ColorPattern colorPattern = ColorPattern::Reset);
}
#endif

namespace Log {
    const std::string find_string_between(const std::string& str, char a, char b);

    template <typename Arg>
    void basic_log(const std::string& pattern, const Arg& arg) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto bstart = ptn.find("{");
        Pattern<std::string>::Log(ptn.substr(0, bstart));
        std::string bs = find_string_between(pattern, '{', '}');
        ptn.erase(0, bstart + bs.length() + 2);

        Pattern<Arg>::Log(arg);
        Pattern<std::string>::Log(ptn);
    }

    template <typename Arg, typename... Args>
    void basic_log(const std::string& pattern, const Arg& arg, const Args& ...args) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto bstart = ptn.find("{");
        Pattern<std::string>::Log(ptn.substr(0, bstart));
        std::string bs = find_string_between(pattern, '{', '}');
        ptn.erase(0, bstart + bs.length() + 2);

        Pattern<Arg>::Log(arg);
        basic_log(pattern, args...);
    }

    void basic_log(const std::string& pattern);

    template <typename... Args>
    void Log(ColorPattern colorPattern, const std::string& pattern, const Args& ...args) {
        SetColor(colorPattern);
        basic_log(pattern, args...);
        SetColor();
    }

    void Log(ColorPattern colorPattern, const std::string& pattern);

}

#define SET_LOG_COLOR_WITH_NAME(name, color)\
        namespace Log {\
            template <typename... Args>\
            void name(const std::string& pattern, const Args& ...args) {\
                Log::Log(color, pattern, args...);\
            }\
        }\

SET_LOG_COLOR_WITH_NAME(Trace, Log::ColorPattern::White);
SET_LOG_COLOR_WITH_NAME(Debug, Log::ColorPattern::Green);
SET_LOG_COLOR_WITH_NAME(Info, Log::ColorPattern::Cyan);
SET_LOG_COLOR_WITH_NAME(Warn, Log::ColorPattern::Yellow);
SET_LOG_COLOR_WITH_NAME(Error, Log::ColorPattern::Magenta);
SET_LOG_COLOR_WITH_NAME(Fatal, Log::ColorPattern::Red);

