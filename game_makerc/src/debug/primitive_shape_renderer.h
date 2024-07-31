#ifndef _DEBUG_PRIMITIVE_SHAPE_RENDERER_
#define _DEBUG_PRIMITIVE_SHAPE_RENDERER_
#include "line_renderer.h"

void draw_debug_circle(vec3 position, float radius, vec3 color);

#if defined DEBUG || defined _DEBUG
	#define DRAW_DEBUG_CIRCLE(position, radius, color) draw_debug_circle(position, radius, color)
#else
	#define DRAW_DEBUG_CIRCLE(position, radius, color)
#endif

#endif
