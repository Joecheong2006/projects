#include "circle2d.h"
#include "core/assert.h"
#include "collision2d_impl.h"

collision2d_info circle2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderCircle2d - ColliderBox2d: return circle2d_box2d_collision_impl(collider1, collider2);
    case ColliderCircle2d - ColliderCircle2d: return circle2d_circle2d_collision_impl(collider1, collider2);
    case ColliderCircle2d - ColliderCapsule2d: return circle2d_capsule2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_info) { .points_count = 0 };
    }
}

f32 get_circle2d_inertia(collider2d* circle_collider) {
    ASSERT(circle_collider != NULL && circle_collider->parent != NULL);
    circle2d* circle = circle_collider->self;
    return circle->radius * circle->radius * 0.5 * circle_collider->parent->mass;
}

