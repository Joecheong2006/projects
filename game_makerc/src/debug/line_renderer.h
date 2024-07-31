#ifndef _DEBUG_LINE_RENDERER_
#define _DEBUG_LINE_RENDERER_
#include <cglm/cglm.h>

void init_debug_line_renderer_instance();
void shutdown_debug_line_renderer();
void render_debug_line(vec3 p1, vec3 p2, vec3 color);

#if defined(DEBUG) || defined(_DEBUG)
	#define INIT_DEBUG_LINE_RENDERER() init_debug_line_renderer_instance()
	#define SHUTDOWN_DEBUG_LINE_RENDERER() shutdown_debug_line_renderer()
	#define DRAW_DEBUG_LINE(p1, p2, color) render_debug_line(p1, p2, color)
#else
	#define INIT_DEBUG_LINE_RENDERER()
	#define SHUTDOWN_DEBUG_LINE_RENDERER()
	#define DRAW_DEBUG_LINE(p1, p2, color)
#endif

#endif
