#include "capsule2d.h"
#include "core/assert.h"
#include "collision2d_impl.h"

collision2d_info capsule2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderCapsule2d - ColliderCircle2d: return capsule2d_circle2d_collision_impl(collider1, collider2);
    case ColliderCapsule2d - ColliderBox2d: return capsule2d_box2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_info) { .points_count = 0 };
    }
}

f32 get_capsule2d_inertia(collider2d* collider) {
	ASSERT_MSG(collider, "invalid collider");
	capsule2d* cap = collider->self;
	float mr = (collider->parent->mass / 12.0f) * ((cap->height * cap->height) + (cap->radius* cap->radius));
	float mc = collider->parent->mass * ((0.5f * cap->radius * cap->radius) + (cap->height * cap->height * 0.25f));
	return mr + (mc * 2.0f);
}
