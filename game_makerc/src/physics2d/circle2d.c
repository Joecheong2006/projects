#include "circle2d.h"
#include "collision2d_impl.h"

collision2d_state circle2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderCircle2d - ColliderBox2d: return circle2d_box2d_collision_impl(collider1, collider2);
    case ColliderCircle2d - ColliderCircle2d: return circle2d_circle2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_state) { .depth = 0 };
    }
}

f32 get_circle2d_inertia(rigid2d* rig, collider2d* circle_collider) {
    ASSERT(circle_collider != NULL && rig != NULL);
    circle2d* circle = circle_collider->self;
    return circle->radius * circle->radius * 0.5 * rig->mass;
}

