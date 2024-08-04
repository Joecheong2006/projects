#ifndef _CORE_ASSERT_H_
#define _CORE_ASSERT_H_

#include "core/log.h"

#if defined(_WIN32)
#include  <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

#if defined(DEBUG)
	#define ASSERT_MSG(x, msg) if (!(x)) { LOG_FATAL(" %s:%d %s %s\n", __FILE__, __LINE__, #x, msg); DEBUG_BREAK(); }
	#define ASSERT(x) ASSERT_MSG(x, "")
#else
	#define ASSERT(x)
	#define ASSERT_MSG(x, msg)
#endif

#endif
