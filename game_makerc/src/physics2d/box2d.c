#include "box2d.h"
#include "collision2d_impl.h"

collision2d_state box2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderBox2d - ColliderBox2d: return box2d_box2d_collision_impl(collider1, collider2);
    case ColliderBox2d - ColliderCircle2d: return box2d_circle2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_state) { .depth = 0 };
    }
}

f32 get_box2d_inertia(rigid2d* rig, collider2d* box_collider) {
    ASSERT(rig != NULL && box_collider != NULL);
    box2d* box = box_collider->self;
    return (box->size[0] * box->size[0] * 4 + box->size[1] * box->size[1] * 4) * rig->mass / 12;
}

