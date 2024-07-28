#ifndef _PHYSICS_CIRCLE2D_
#define _PHYSICS_CIRCLE2D_
#include "collider2d.h"

typedef struct {
    vec2 center;
    float radius;
} circle2d;

collision2d_state circle2d_collision_callback(collider2d* collider1, collider2d* collider2);
collision2d_state circle2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_state circle2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2);

#endif
