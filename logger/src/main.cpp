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

#define testing_type char*

// %[flags][width].[percision][specifier]

int main() {
    vec3f v(6.1, 3.04, 9.889);

    //Log::Trace(TEST_MESSAGES);
    //Log::Debug(TEST_MESSAGES);
    //Log::Info(TEST_MESSAGES);
    //Log::Warn(TEST_MESSAGES);
    //Log::Error(TEST_MESSAGES);
    //Log::Fatal(TEST_MESSAGES);

    const testing_type i = "hi";

    Log::Info("{}\n", i);

    Log::Trace("end\n");
}
