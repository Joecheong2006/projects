#include <stdio.h>
#include <string>

// Pattern '{1}, {0}'  Args
// <------ Formatter ------>
// Format '%f, %s'    Args

namespace Log {
    template <typename T>
    struct Pattern {
        explicit Pattern() {}
        static void Log(const T& value) { (void)value; }
    };

#define __DEFINE_PATTERN_BASIC_LOG(type, pattern, ...)\
        template <>\
        struct Pattern<type> {\
            static void Log(const type& value) {\
                printf(pattern, __VA_ARGS__);\
            }\
        };\

__DEFINE_PATTERN_BASIC_LOG(int, "%d", value);
__DEFINE_PATTERN_BASIC_LOG(float, "%g", value);
__DEFINE_PATTERN_BASIC_LOG(double, "%lg", value);
__DEFINE_PATTERN_BASIC_LOG(char*, "%s", value);
__DEFINE_PATTERN_BASIC_LOG(char, "%c", value);
__DEFINE_PATTERN_BASIC_LOG(std::string, "%s", value.c_str());

    template <typename Arg>
    void info(const std::string& pattern, const Arg& arg) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto fristIndex = ptn.find("{");
        auto endIndex = ptn.find("}");
        for (size_t i = 0; i <= endIndex; ++i)  {
            if (i < fristIndex) {
                Pattern<char>::Log(ptn[0]);
                ptn.erase(0, 1);
                continue;
            }
            ptn.erase(0, 1);
        }
        Pattern<Arg>::Log(arg);
    }

    template <typename Arg, typename... Args>
    void info(const std::string& pattern, const Arg& arg, const Args& ...args) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto fristIndex = ptn.find("{");
        auto endIndex = ptn.find("}");
        for (size_t i = 0; i <= endIndex; ++i)  {
            if (i < fristIndex) {
                Pattern<char>::Log(ptn[0]);
                ptn.erase(0, 1);
                continue;
            }
            ptn.erase(0, 1);
        }
        Pattern<Arg>::Log(arg);
        info(ptn, args...);
    }
}

#define DEFINE_PATTERN_BASIC_LOG(type, pattern, ...)\
        template <>\
        struct Log::Pattern<type> {\
            static void Log(const type& value) {\
                printf(pattern, __VA_ARGS__);\
            }\
        };\

#define DEFINE_PATTERN_BASIC_LOGLN(type, pattern, ...)\
    DEFINE_PATTERN_BASIC_LOG(type, pattern "\n", __VA_ARGS__);\

template <typename T>
struct vec3 {
    friend struct Log::Pattern<vec3<float>>;
    vec3()
    {}

    vec3(T x, T y, T z)
        : x(x), y(y), z(z)
    {}

private:
    T x, y, z;

};

DEFINE_PATTERN_BASIC_LOG(vec3<float>, "[%.3g, %.3g, %.3g]", value.x, value.y, value.z)

#define LOG_INFO(pattern, ...)\
    Log::info(pattern, __VA_ARGS__);

int main() {
    vec3<float> v(6, 3, 9);
    vec3<float> v1(1, 2, 3);
    LOG_INFO("v:{} v1:{} int:{} double:{} float:{}", v, v1, 10, 2.405, 4.3f);
    return 0;
}
