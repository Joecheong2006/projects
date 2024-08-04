#ifndef _CORE_DEFIENS_H_
#define _CORE_DEFIENS_H_

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

_Static_assert(sizeof(u8) == 1, "expected u8 to be 1 byte");
_Static_assert(sizeof(i8) == 1, "expected i8 to be 1 byte");
_Static_assert(sizeof(u16) == 2, "expected u16 to be 2 byte");
_Static_assert(sizeof(i16) == 2, "expected i16 to be 2 byte");
_Static_assert(sizeof(u32) == 4, "expected u32 to be 4 byte");
_Static_assert(sizeof(i32) == 4, "expected i32 to be 4 byte");
_Static_assert(sizeof(u64) == 8, "expected u64 to be 8 byte");
_Static_assert(sizeof(i64) == 8, "expected i64 to be 8 byte");
_Static_assert(sizeof(f32) == 4, "expected f32 to be 4 byte");
_Static_assert(sizeof(f64) == 8, "expected f64 to be 4 byte");

#if defined(_WIN32)
#include  <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

#if defined(DEBUG)
	#include "core/log.h"
	#define ASSERT_MSG(x, msg) if (!(x)) { log_msg(LogLevelFatal, " %s:%d %s %s\n", __FILE__, __LINE__, #x, msg); DEBUG_BREAK(); }
	#define ASSERT(x) ASSERT_MSG(x, "")
#else
	#define ASSERT(x)
	#define ASSERT_MSG(x, msg)
#endif

#endif
