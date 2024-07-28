#include "collider2d.h"
#include "box2d.h"
#include "circle2d.h"

static collider2d_collision_callback collider2d_collision_callback_table[] = {
	[ColliderBox2d] = box2d_collision_callback,
	[ColliderCircle2d] = circle2d_collision_callback
};

collider2d create_collider2d(Collider2dType type, rigid2d* parent, void* context) {
    return (collider2d) {
        .type = type,
        .collide = collider2d_collision_callback_table[type],
        .self = context,
        .parent = parent,
    };
}
