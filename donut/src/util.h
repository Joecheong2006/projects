#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

#define time_count(line)                                                    \
    {                                                                       \
        clock_t before = clock();                                           \
        line                                                                \
        clock_t current = clock() - before;                                 \
        printf("taked time = %ldms\n", current * 1000 / CLOCKS_PER_SEC);    \
    }                                                                       \

#define time_count_for(count, line)                                             \
    {                                                                           \
        clock_t before = clock();                                               \
        for(u64 _current_count = 0; _current_count < count; _current_count++)   \
        {                                                                       \
            line                                                                \
        }                                                                       \
        clock_t current = clock() - before;                                     \
        printf("taked time = %ldms\n", current * 1000 / CLOCKS_PER_SEC);        \
    }                                                                           \

#endif
