#ifndef _PHYSICS_BOX2D_
#define _PHYSICS_BOX2D_
#include "collider2d.h"

typedef struct {
    vec2 size, center;
} box2d;

collision2d_info box2d_collision_callback(collider2d* collider1, collider2d* collider2);
f32 get_box2d_inertia(collider2d* box_collider);

#endif
