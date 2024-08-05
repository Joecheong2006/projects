#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "core/defines.h"

typedef struct {
	i32(*on_initialize)(void* app);
	i32(*is_running)(void* app);
	void(*on_update)(void* app);
	void(*on_terminate)(void* app);
	void* app;
} application_setup;

#endif
