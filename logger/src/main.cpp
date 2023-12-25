#include <iostream>
#include <string>
#include <tuple>
#include <vector>

namespace Log {
    struct Pattern {
        Pattern(const std::string& ptn)
            : pattern(ptn)
        {}

        void operator=(const std::string& ptn) {
            pattern = ptn;
        }

        std::string pattern;
    };

    template <typename... Args>
    struct Format {
        explicit Format(const std::string& ptn)
            : ptn(ptn)
        {}

        Pattern ptn;
        std::vector<void*> parameter;
    };

    template <typename T>
    struct Formatter {
        Format<int> format(T value) {
            return Format<int>("", value);
        }

        void print_parameter() {
        }
    };

    template <typename T>
    void Demo(T value) {
        Formatter<T> fmt;
        std::cout << fmt.format(value).ptn.pattern;
    }
}

struct vec2 {
    vec2(int x, int y)
        : x(x), y(y)
    {}

    friend Log::Formatter<vec2>;
private:
    int x, y;
};

template <>
struct Log::Formatter<vec2> {
    Format<int, int> format(vec2 v) {
        Format<int, int> format("vec2[{}, {}]");
        return format;
    }
};

int main() {
    //print_parameter<int, float>(vparameter);
    //auto vec = parameter<int>(1, 2, 3);
    //Log::Demo<vec2>(vec2(1, 2));
}
