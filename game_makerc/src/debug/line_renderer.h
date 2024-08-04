#ifndef _DEBUG_LINE_RENDERER_H_
#define _DEBUG_LINE_RENDERER_H_
#include <cglm/cglm.h>

void init_debug_line_renderer();
void shutdown_debug_line_renderer();
void render_debug_line(vec3 p1, vec3 p2, vec3 color);

#endif
