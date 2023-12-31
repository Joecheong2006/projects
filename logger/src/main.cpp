#include "logger.h"
#include <vector>

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

template <typename T>
struct Log::Pattern<vec3<T>> {
    static void Log(const vec3<T>& value, std::string format) {
        (void)format;
        Log::Log("[{}, {}, {}]", value.x, value.y, value.z);
    }
};

#define TEST_MESSAGES "v:{} int:{} double:{} float:{}\n", v, 10, 2.05, 4.3f

typedef double testing_type;

// %[flags][width].[percision][specifier]

int main() {
    vec3f v(6.1, 3.04, 9.889);

    std::vector<std::vector<testing_type>> vec = {
        std::vector<testing_type>{ 1.1, 2.2, 3.1 },
        std::vector<testing_type>{ 4.2, 5.3, 6.2 },
        std::vector<testing_type>{ 7.2, 8.4, 9.3 },
    };
    Log::Trace("{}\n", vec);

    Log::Trace(TEST_MESSAGES);
    Log::Debug(TEST_MESSAGES);
    Log::Info(TEST_MESSAGES);
    Log::Warn(TEST_MESSAGES);
    Log::Error(TEST_MESSAGES);
    Log::Fatal(TEST_MESSAGES);

    Log::Info("{:10.2}\n", 10.1);

    Log::Trace("end\n");
}
