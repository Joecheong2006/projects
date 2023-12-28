#include "logger.h"

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

#define TEST_MESSAGES "v: {} v1: {} int: {} double: {} float: {}\n", v, v1, 10, 2.405, 4.3f

int main() {
    vec3f v(6.1, 3.04, 9.889);
    vec3i v1(1, 2, 3);

    Log::Trace(TEST_MESSAGES);
    Log::Debug(TEST_MESSAGES);
    Log::Info(TEST_MESSAGES);
    Log::Warn(TEST_MESSAGES);
    Log::Error(TEST_MESSAGES);
    Log::Fatal(TEST_MESSAGES);

    Log::Trace("end\n");
}
