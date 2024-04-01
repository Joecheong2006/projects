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
#define TEST_LOG_PATTERN(value)\
    Log::Info(#value ": {:022}\n", value);\
    Log::Info(#value ": {:<22}\n", value);\
    Log::Info(#value ": {:>22}\n", value);\


// %[flags][width].[percision][specifier]

int main() {
    const int8_t i8 = 22;
    const uint8_t u8 = -1;
    const int16_t i16 = 14;
    const uint16_t u16 = -1;
    const int32_t i32 = 22;
    const uint32_t u32 = -1;
    const long l = 22;
    const unsigned long lu = -1;
    const int64_t i64 = 22;
    const uint64_t u64 = -1;

    TEST_LOG_PATTERN(i8);
    TEST_LOG_PATTERN(u8);
    TEST_LOG_PATTERN(i16);
    TEST_LOG_PATTERN(u16);
    TEST_LOG_PATTERN(i32);
    TEST_LOG_PATTERN(u32);
    TEST_LOG_PATTERN(l);
    TEST_LOG_PATTERN(lu);
    TEST_LOG_PATTERN(i64);
    TEST_LOG_PATTERN(u64);

    vec3f v(6.1, 3.04, 9.889);

    Log::Trace(TEST_MESSAGES);
    Log::Debug(TEST_MESSAGES);
    Log::Info(TEST_MESSAGES);
    Log::Warn(TEST_MESSAGES);
    Log::Error(TEST_MESSAGES);
    Log::Fatal(TEST_MESSAGES);

    Log::Trace("end\n");
}
