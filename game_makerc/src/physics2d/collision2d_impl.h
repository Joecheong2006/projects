#ifndef _PHYSICS_COLLISION2D_H_
#define _PHYSICS_COLLISION2D_H_
#include "collider2d.h"

collision2d_info box2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info circle2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info box2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info circle2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info circle2d_capsule2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info capsule2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info box2d_capsule2d_collision_impl(collider2d* collider1, collider2d* collider2);
collision2d_info capsule2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2);

#endif
