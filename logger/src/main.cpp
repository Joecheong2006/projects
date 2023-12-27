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

    enum ColorPattern : unsigned char {
        Reset = 0,
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
    };

    void SetColor(unsigned char colorPattern) { 
        std::string pattern = "\033[1;" + std::to_string(colorPattern) + "m";
        Pattern<std::string>::Log(pattern); 
    }

    void SetColor(ColorPattern colorPattern = Reset) { 
        SetColor((unsigned char)colorPattern);
    }

    const std::string find_string_between(const std::string& str, char a, char b) {
        std::string result;
        auto fristIndex = str.find(a);
        auto endIndex = str.find(b);
        for (size_t i = fristIndex + 1; i < endIndex; ++i)
            result += str[i];
        return result;
    }

    template <typename Arg>
    void Info(const std::string& pattern, const Arg& arg) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto bstart = ptn.find("{");
        for (unsigned long long i = 0; i < bstart; ++i) {
            Pattern<char>::Log(ptn[0]);
            ptn.erase(0, 1);
        }
        std::string bs = find_string_between(pattern, '{', '}');
        ptn.erase(0, bs.length() + 2);

        Pattern<Arg>::Log(arg);
        Pattern<std::string>::Log(ptn);
    }

    template <typename Arg, typename... Args>
    void Info(const std::string& pattern, const Arg& arg, const Args& ...args) {
        std::string& ptn = const_cast<std::string&>(pattern);
        auto bstart = ptn.find("{");
        for (unsigned long long i = 0; i < bstart; ++i) {
            Pattern<char>::Log(ptn[0]);
            ptn.erase(0, 1);
        }
        std::string bs = find_string_between(pattern, '{', '}');
        ptn.erase(0, bs.length() + 2);

        Pattern<Arg>::Log(arg);
        Info(pattern, args...);
    }

    void Info(const std::string& pattern) {
        Pattern<std::string>::Log(pattern);
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
    friend struct Log::Pattern<vec3>;
    vec3()
    {}

    vec3(T x, T y, T z)
        : x(x), y(y), z(z)
    {}

private:
    T x, y, z;

};

typedef vec3<float> vec3f;
typedef vec3<int> vec3i;

DEFINE_PATTERN_BASIC_LOG(vec3f, "[%.3g, %.3g, %.3g]", value.x, value.y, value.z)
DEFINE_PATTERN_BASIC_LOG(vec3i, "[%d, %d, %d]", value.x, value.y, value.z)

int main() {
    vec3f v(6.1, 3.04, 9.889);
    vec3i v1(1, 2, 3);
    for (int i = 0; i < 8; ++i) {
        Log::SetColor((unsigned char)(30 + i));
        Log::Info("v: {} v1: {} int: {} double: {} float: {}\n", v, v1, 10, 2.405, 4.3f);
    }
    Log::Info("end");
    Log::SetColor();
}
