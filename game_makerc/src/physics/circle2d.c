#include "circle2d.h"
#include "box2d.h"

collision2d_state circle2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c = collider1->self;
    box2d* b = collider2->self;
    return (collision2d_state) {
        .depth = glm_vec2_distance(c->center, b->center),
    };
}

collision2d_state circle2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c1 = collider1->self;
    circle2d* c2 = collider2->self;

    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, c1->center, p1);
    glm_vec2_add(collider2->parent->tran->position, c2->center, p2);
    const f32 distance = glm_vec2_distance(p1, p2);

    f32 depth = distance - c1->radius - c2->radius;
    vec2 normal;
    glm_vec2_sub(p2, p1, normal);
    glm_vec2_divs(normal, distance, normal);

    return (collision2d_state) {
        .depth = -depth,
        .normal = {normal[0], normal[1]},
        .contact = { normal[0] * (c1->radius + depth * 0.5), normal[1] * (c1->radius + depth * 0.5)}
    };
}

collision2d_state circle2d_collision_callback(collider2d* collider1, collider2d* collider2) {
    switch (collider1->type - collider2->type) {
    case ColliderCircle2d - ColliderBox2d: return circle2d_box2d_collision_impl(collider1, collider2);
    case ColliderCircle2d - ColliderCircle2d: return circle2d_circle2d_collision_impl(collider1, collider2);
    default:
        printf("not implement %d %d yet\n", collider1->type, collider2->type);
        return (collision2d_state) { .depth = 0 };
    }
}