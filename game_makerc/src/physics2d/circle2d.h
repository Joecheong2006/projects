#ifndef _PHYSICS_CIRCLE2D_H_
#define _PHYSICS_CIRCLE2D_H_
#include "collider2d.h"

typedef struct {
    vec2 center;
    f32 radius;
} circle2d;

collision2d_info circle2d_collision_callback(collider2d* collider1, collider2d* collider2);
f32 get_circle2d_inertia(collider2d* circle_collider);

#endif
