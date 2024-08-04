#ifndef _PHYSICS_CAPSULE2D_H_
#define _PHYSICS_CAPSULE2D_H_
#include "collider2d.h"

typedef struct {
	vec2 center;
	f32 height, radius;
	i32 horizontal;
} capsule2d;

collision2d_info capsule2d_collision_callback(collider2d* collider1, collider2d* collider2);
f32 get_capsule2d_inertia(collider2d* collider);

#endif
