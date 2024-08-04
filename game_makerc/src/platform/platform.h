#ifndef _PLATFORM_PLATFORM_H_
#define _PLATFORM_PLATFORM_H_
#include "core/defines.h"

typedef struct {
	void* state;
} platform_state;

i32 setup_platform(platform_state* target);
void shutdown_platform(platform_state* target);

f64 platform_get_time();
void platform_sleep(i32 ms);
void platform_console_log(const char* msg, i32 color);

#endif
