#include "box2d.h"
#include "circle2d.h"

collision2d_state box2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    box2d* b1 = collider1->self;
    box2d* b2 = collider2->self;
    return (collision2d_state) {
        .depth = glm_vec2_distance(b1->center, b2->center),
    };
}

collision2d_state box2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderBox2d - ColliderBox2d: return box2d_box2d_collision_impl(collider1, collider2);
    case ColliderBox2d - ColliderCircle2d: return circle2d_box2d_collision_impl(collider2, collider1);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_state) { .depth = 0 };
    }
}

