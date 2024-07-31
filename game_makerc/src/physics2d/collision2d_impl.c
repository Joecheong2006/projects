#include "collision2d_impl.h"
#include "box2d.h"
#include "circle2d.h"

#include "../debug/primitive_shape_renderer.h"

static void find_closest_point_on_line(vec2 closest, vec2 line_p1, vec2 line_p2, vec2 p) {
    vec2 tangent, line_normal;
    f32 line_len = glm_vec2_distance(line_p1, line_p2);
    glm_vec2_sub(p, line_p1, tangent);
    glm_vec2_sub(line_p2, line_p1, line_normal);
    glm_vec2_divs(line_normal, line_len, line_normal);
    f32 stand_scalar = glm_clamp(glm_vec2_dot(tangent, line_normal), 0, line_len);
    closest[0] = line_normal[0] * stand_scalar + line_p1[0];
    closest[1] = line_normal[1] * stand_scalar + line_p1[1];
}

collision2d_state box2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    box2d* b1 = collider1->self;
    box2d* b2 = collider2->self;

    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, b1->center, p1);
    glm_vec2_add(collider2->parent->tran->position, b2->center, p2);

    vec2 bp1[] = {
        {  b1->size[0],  b1->size[1], },
        { -b1->size[0],  b1->size[1], },
        { -b1->size[0], -b1->size[1], },
        {  b1->size[0], -b1->size[1] }
    };

    for (int i = 0; i < 4; i++) {
        f32 angle = collider1->parent->tran->euler_angle[2];
        glm_vec2_rotate(bp1[i], angle, bp1[i]);
        glm_vec2_add(p1, bp1[i], bp1[i]);
    }

    vec2 bp2[] = {
        {  b2->size[0],  b2->size[1], },
        { -b2->size[0],  b2->size[1], },
        { -b2->size[0], -b2->size[1], },
        {  b2->size[0], -b2->size[1] }
    };

    for (int i = 0; i < 4; i++) {
        f32 angle = collider2->parent->tran->euler_angle[2];
        glm_vec2_rotate(bp2[i], angle, bp2[i]);
        glm_vec2_add(p2, bp2[i], bp2[i]);
    }

    vec2 closest_point[4];
    f32 closest_dis[4];
    for (int i = 0; i < 4; i++) {
        find_closest_point_on_line(closest_point[i], bp2[0], bp2[1], bp1[i]);
        closest_dis[i] = glm_vec2_distance(bp1[i], p1);
        for (int j = 1; j < 4; j++) {
            vec2 p;
            find_closest_point_on_line(p, bp2[j], bp2[(j + 1) % 4], bp1[i]);
            f32 dis = glm_vec2_distance(bp1[i], p);
            if (dis < closest_dis[i]) {
                closest_dis[i] = dis;
                glm_vec2_copy(p, closest_point[i]);
            }
        }
    }

    int count = 0;
    vec3 contact_point[2];
    vec2 normals[2];

    for (int i = 0; i < 4; i++) {
        vec2 normal, pn;
        glm_vec2_sub(closest_point[i], p1, normal);
        glm_vec2_sub(closest_point[i], bp1[i], pn);
        if (glm_vec2_dot(normal, pn) < 0) {
            glm_vec2_copy(closest_point[i], contact_point[count]);
            contact_point[count][2] = closest_dis[i];
            glm_vec2_copy(normal, normals[count]);
            count++;
        }
    }

    collision2d_state state;
    state.points_count = count;
    for (int i = 0; i < count; i++) {
        state.collision_points[i] = (collision2d_point) {
            .contact = { contact_point[i][0], contact_point[i][1] },
            .depth = contact_point[i][2],
            .normal = { normals[i][0], normals[i][1] }
        };
    }

    return state;
}

collision2d_state circle2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c = collider1->self;
    box2d* b = collider2->self;
    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, c->center, p1);
    glm_vec2_add(collider2->parent->tran->position, b->center, p2);

    vec2 top_right = {
        b->size[0], b->size[1]
    },
    top_left = {
        -b->size[0], b->size[1]
    },
    bottom_left = {
        -b->size[0], -b->size[1]
    },
    bottom_right = {
        b->size[0], -b->size[1]
    };

    f32 angle = collider2->parent->tran->euler_angle[2];
    glm_vec2_rotate(top_right, angle, top_right);
    glm_vec2_rotate(top_left, angle, top_left);
    glm_vec2_rotate(bottom_right, angle, bottom_right);
    glm_vec2_rotate(bottom_left, angle, bottom_left);

    glm_vec2_add(p2, top_right, top_right);
    glm_vec2_add(p2, top_left, top_left);
    glm_vec2_add(p2, bottom_right, bottom_right);
    glm_vec2_add(p2, bottom_left, bottom_left);

    vec2 closest, tempv;
    f32 distance, temp;
    find_closest_point_on_line(closest, top_left, top_right, p1);
    distance = glm_vec2_distance(closest, p1);
    temp = distance;
    glm_vec2_copy(closest, tempv);

    find_closest_point_on_line(closest, top_right, bottom_right, p1);
    distance = glm_vec2_distance(closest, p1);
    if (temp < distance) {
        distance = temp;
        glm_vec2_copy(tempv, closest);
    }
    else {
        temp = distance;
        glm_vec2_copy(closest, tempv);
    }

    find_closest_point_on_line(closest, bottom_right, bottom_left, p1);
    distance = glm_vec2_distance(closest, p1);
    if (temp < distance) {
        distance = temp;
        glm_vec2_copy(tempv, closest);
    }
    else {
        temp = distance;
        glm_vec2_copy(closest, tempv);
    }

    find_closest_point_on_line(closest, bottom_left, top_left, p1);
    distance = glm_vec2_distance(closest, p1);
    if (temp < distance) {
        distance = temp;
        glm_vec2_copy(tempv, closest);
    }
    else {
        temp = distance;
        glm_vec2_copy(closest, tempv);
    }

    vec2 normal;
    glm_vec2_sub(closest, p1, normal);
    glm_vec2_normalize(normal);
    f32 depth = c->radius - distance;

    return (collision2d_state) {
        .collision_points[0] = {
            .depth = depth,
            .normal = {normal[0], normal[1]},
            .contact = {closest[0], closest[1]},
        },
        .points_count = 1,
    };
}

collision2d_state box2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    collision2d_state state = circle2d_box2d_collision_impl(collider2, collider1);
    state.collision_points[0].normal[0] *= -1;
    state.collision_points[0].normal[1] *= -1;
    return state;
}

collision2d_state circle2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c1 = collider1->self;
    circle2d* c2 = collider2->self;

    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, c1->center, p1);
    glm_vec2_add(collider2->parent->tran->position, c2->center, p2);
    const f32 distance = glm_vec2_distance(p1, p2);

    f32 depth = c1->radius + c2->radius - distance;
    vec2 normal;
    glm_vec2_sub(p2, p1, normal);
    glm_vec2_divs(normal, distance, normal);

    return (collision2d_state) {
        .collision_points[0] = {
            .depth = depth,
            .normal = {normal[0], normal[1]},
            .contact = { p1[0] + normal[0] * c1->radius, p1[1] + normal[1] * c1->radius}
        },
        .points_count = 1,
    };
}

