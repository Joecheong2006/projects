#ifndef _PHYSICS_CYLINDER2D_
#define _PHYSICS_CYLINDER2D_
#include "collider2d.h"

typedef struct {
	vec2 center, size;
} capsule2d;

collision2d_state cylinder2d_collision_callback(collider2d* collider1, collider2d* collider2);
collision2d_state cylinder2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2);

#endif
