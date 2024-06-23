#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

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

#define INLINE inline __attribute__((always_inline))
#define PACKED __attribute__((packed))

#if defined(DEBUG)
#include <stdio.h>
#define ASSERT_MSG(x, msg)\
    if (!(x)) {\
        printf("%s in %s from %s at %d\n", msg, __FILE__, __func__, __LINE__);\
        exit(1);}
#else
#define ASSERT_MSG(x, msg)
#endif

#endif
