#ifndef _PHYSICS_BOX2D_
#define _PHYSICS_BOX2D_
#include "collider2d.h"

typedef struct {
    vec2 size, center;
} box2d;

collision2d_state box2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_state box2d_collision_callback(collider2d* collider1, collider2d* collider2);

#endif
