#include "collider2d.h"
#include "box2d.h"
#include "circle2d.h"

static collider2d_collision_callback collider2d_collision_callback_table[] = {
	[ColliderBox2d] = box2d_collision_callback,
	[ColliderCircle2d] = circle2d_collision_callback
};

static collider2d_get_inertia_callback get_inertia_table[] = {
	[ColliderBox2d] = get_box2d_inertia,
	[ColliderCircle2d] = get_circle2d_inertia,
};

collider2d create_collider2d(Collider2dType type, rigid2d* parent, void* context) {
	collider2d collider = {
        .type = type,
        .collide = collider2d_collision_callback_table[type],
        .get_inertia = get_inertia_table[type],
        .self = context,
        .parent = parent,
	};
	parent->inertia = collider.get_inertia(&collider);
	parent->inverse_inertia = 1.0 / parent->inertia;
	return collider;
}
