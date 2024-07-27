#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

#if defined(DEBUG)
	#if defined(WIN32)
		#define ASSERT(x) if (!(x)) __debugbreak()
	#else
		#include <assert.h>
		#define ASSERT(x) assert(x);
	#endif
#else
	#define ASSERT(x)
#endif

#endif
