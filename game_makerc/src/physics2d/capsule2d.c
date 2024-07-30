#include "capsule2d.h"
#include "circle2d.h"

collision2d_state cylinder2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderCylinder2d - ColliderCircle2d: return cylinder2d_circle2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_state) { .depth = 0 };
    }
}

collision2d_state cylinder2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
	capsule2d* cy = collider1->self;
	circle2d* c = collider2->self;
	vec2 cy_p1, cy_p2;
	// glm_vec2_copy();

	return (collision2d_state) {
		.depth = 0,
	};
}

