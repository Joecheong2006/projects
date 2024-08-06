#include "collision2d_impl.h"
#include "box2d.h"
#include "circle2d.h"
#include "capsule2d.h"
#include <math.h>

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

static void project_points(f32* min, f32* max, vec2* v, int c, vec2 t) {
    *min = FLT_MAX;
    *max = FLT_MIN;
    for (i32 i = 0; i < c; ++i) {
        f32 proj = glm_vec2_dot(v[i], t);
        if (proj > *max) {
            *max = proj;
        }
        if (proj < *min) {
            *min = proj;
        }
    }
}

static i32 intersect_polygons(vec2 normal, f32* depth, vec2* v1, i32 c1, vec2* v2, i32 c2) {
    *depth = FLT_MAX;
    for (i32 i = 0; i < c1; ++i) {
        vec2 line;
        glm_vec2_sub(v1[i], v1[(i + 1) % c1], line);
        vec2 tangent = {-line[1], line[0]};
        f32 min1, max1, min2, max2;
        glm_vec2_normalize(tangent);
        project_points(&min1, &max1, v1, c1, tangent);
        project_points(&min2, &max2, v2, c2, tangent);
        if (min1 >= max2 || min2 >= max1) {
            return 0;
        }
        f32 tangent_depth = glm_min(max2 - min1, max1 - min2);
        if (*depth > tangent_depth) {
            *depth = tangent_depth;
            glm_vec2_copy(tangent, normal);
        }
    }

    for (i32 i = 0; i < c2; ++i) {
        vec2 line;
        glm_vec2_sub(v2[i], v2[(i + 1) % c2], line);
        vec2 tangent = {-line[1], line[0]};
        f32 min1, max1, min2, max2;
        glm_vec2_normalize(tangent);
        project_points(&min1, &max1, v1, c1, tangent);
        project_points(&min2, &max2, v2, c2, tangent);
        if (min1 >= max2 || min2 >= max1) {
            return 0;
        }
        f32 tangent_depth = glm_min(max2 - min1, max1 - min2);
        if (*depth > tangent_depth) {
            *depth = tangent_depth;
            glm_vec2_copy(tangent, normal);
        }
    }

    return 1;
}

static i32 nearly_equal(f32 a, f32 b) {
    return fabs(a - b) - 0.0005 < 0;
}

static i32 nearly_equalv(vec2 a, vec2 b) {
    return glm_vec2_distance(a, b) < 0.0005;
}

collision2d_info box2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    box2d* b1 = collider1->self;
    box2d* b2 = collider2->self;

    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, b1->center, p1);
    glm_vec2_add(collider2->parent->tran->position, b2->center, p2);

    vec2 vp1[] = {
        {  b1->size[0],  b1->size[1], },
        { -b1->size[0],  b1->size[1], },
        { -b1->size[0], -b1->size[1], },
        {  b1->size[0], -b1->size[1] }
    };

    for (i32 i = 0; i < 4; i++) {
        glm_vec2_rotate(vp1[i], collider1->parent->tran->euler_angle[2], vp1[i]);
        glm_vec2_add(p1, vp1[i], vp1[i]);
    }

    vec2 vp2[] = {
        {  b2->size[0],  b2->size[1], },
        { -b2->size[0],  b2->size[1], },
        { -b2->size[0], -b2->size[1], },
        {  b2->size[0], -b2->size[1] }
    };

    for (i32 i = 0; i < 4; i++) {
        glm_vec2_rotate(vp2[i], collider2->parent->tran->euler_angle[2], vp2[i]);
        glm_vec2_add(p2, vp2[i], vp2[i]);
    }

    vec2 normal;
    f32 depth;
    if (!intersect_polygons(normal, &depth, vp1, 4, vp2, 4)) {
        return (collision2d_info) {
            .points_count = 0,
        };
    }

    {
        vec2 v12;
        glm_vec2_sub(p2, p1, v12);
        if (glm_vec2_dot(v12, normal) < 0) {
            normal[0] *= -1;
            normal[1] *= -1;
        }
    }

    i32 count = 0;
    f32 min_dis = FLT_MAX;

    vec2 contact_point[2];

    for (i32 i = 0; i < 4; ++i) {
        for (i32 j = 0; j < 4; ++j) {
            vec2 cp;
            find_closest_point_on_line(cp, vp2[j], vp2[(j + 1) % 4], vp1[i]);
            f32 dis = glm_vec2_distance(vp1[i], cp);
            if (nearly_equal(dis, min_dis)) {
                if (!nearly_equalv(cp, contact_point[0])) {
                    glm_vec2_copy(cp, contact_point[1]);
                    count = 2;
                }
            }
            else if (dis < min_dis) {
                glm_vec2_copy(cp, contact_point[0]);
                min_dis = dis;
                count = 1;
            }
        }
    }

    for (i32 i = 0; i < 4; ++i) {
        for (i32 j = 0; j < 4; ++j) {
            vec2 cp;
            find_closest_point_on_line(cp, vp1[j], vp1[(j + 1) % 4], vp2[i]);
            f32 dis = glm_vec2_distance(vp2[i], cp);
            if (nearly_equal(dis, min_dis)) {
                if (!nearly_equalv(cp, contact_point[0])) {
                    glm_vec2_copy(cp, contact_point[1]);
                    count = 2;
                }
            }
            else if (dis < min_dis) {
                glm_vec2_copy(cp, contact_point[0]);
                min_dis = dis;
                count = 1;
            }
        }
    }

    collision2d_info info;
    info.points_count = count;
    for (i32 i = 0; i < count; ++i) {
        info.collision_points[i] = (collision2d_point) {
            .contact = { contact_point[i][0], contact_point[i][1] },
            .depth = depth,
            .normal = { normal[0], normal[1] }
        };
    }

    return info;
}

collision2d_info circle2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c = collider1->self;
    box2d* b = collider2->self;
    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, c->center, p1);
    glm_vec2_add(collider2->parent->tran->position, b->center, p2);

    vec2 vp[] = {
        {  b->size[0],  b->size[1], },
        { -b->size[0],  b->size[1], },
        { -b->size[0], -b->size[1], },
        {  b->size[0], -b->size[1] }
    };

    for (i32 i = 0; i < 4; i++) {
        glm_vec2_rotate(vp[i], collider2->parent->tran->euler_angle[2], vp[i]);
        glm_vec2_add(p2, vp[i], vp[i]);
    }

    f32 min_dis = FLT_MAX;
    vec2 contact_point;

    for (i32 i = 0; i < 4; ++i) {
        vec2 cp;
        find_closest_point_on_line(cp, vp[i], vp[(i + 1) % 4], p1);
        f32 dis = glm_vec2_distance(cp, p1);
        if (dis < min_dis) {
            glm_vec2_copy(cp, contact_point);
            min_dis = dis;
        }
    }

    vec2 normal;
    glm_vec2_sub(contact_point, p1, normal);
    glm_vec2_normalize(normal);
    f32 depth = c->radius - min_dis;

    return (collision2d_info) {
        .collision_points[0] = {
            .depth = depth,
            .normal = {normal[0], normal[1]},
            .contact = {contact_point[0], contact_point[1]},
        },
        .points_count = 1,
    };
}

collision2d_info box2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    collision2d_info info = circle2d_box2d_collision_impl(collider2, collider1);
    info.collision_points[0].normal[0] *= -1;
    info.collision_points[0].normal[1] *= -1;
    return info;
}

collision2d_info circle2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
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

    return (collision2d_info) {
        .collision_points[0] = {
            .depth = depth,
            .normal = {normal[0], normal[1]},
            .contact = { p1[0] + normal[0] * c1->radius, p1[1] + normal[1] * c1->radius}
        },
        .points_count = 1,
    };
}

collision2d_info circle2d_capsule2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    circle2d* c = collider1->self;
    capsule2d* cap = collider2->self;
    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, c->center, p1);
    glm_vec2_add(collider2->parent->tran->position, cap->center, p2);

    i32 is_hori = cap->horizontal;
    vec2 cap_p1 = {-cap->height * 0.5 * is_hori, -cap->height * 0.5 * (1 - is_hori)};
    vec2 cap_p2 = {cap->height * 0.5 * is_hori, cap->height * 0.5 * (1 - is_hori)};

    glm_vec2_rotate(cap_p1, collider2->parent->tran->euler_angle[2], cap_p1);
    glm_vec2_rotate(cap_p2, collider2->parent->tran->euler_angle[2], cap_p2);

    glm_vec2_add(cap_p1, p2, cap_p1);
    glm_vec2_add(cap_p2, p2, cap_p2);

    vec2 contact;
    find_closest_point_on_line(contact, cap_p1, cap_p2, p1);

    vec2 normal;
    f32 distance = glm_vec2_distance(contact, p1);
    glm_vec2_sub(contact, p1, normal);
    glm_vec2_divs(normal, distance, normal);

    return (collision2d_info) {
        .collision_points[0] = {
            .depth = c->radius + cap->radius - distance,
            .normal = {normal[0], normal[1]},
            .contact = {p1[0] + normal[0] * c->radius, p1[1] + normal[1] * c->radius},
        },
        .points_count = 1,
    };
}

collision2d_info capsule2d_circle2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    collision2d_info info = circle2d_capsule2d_collision_impl(collider2, collider1);
    info.collision_points[0].normal[0] *= -1;
    info.collision_points[0].normal[1] *= -1;
    return info;
}

collision2d_info box2d_capsule2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    box2d* box = collider1->self;
    capsule2d* cap = collider2->self;
    vec2 p1, p2;
    glm_vec2_add(collider1->parent->tran->position, box->center, p1);
    glm_vec2_add(collider2->parent->tran->position, cap->center, p2);

    i32 is_hori = cap->horizontal;
    vec2 cap_p1 = {-cap->height * 0.5 * is_hori, -cap->height * 0.5 * (1 - is_hori)};
    vec2 cap_p2 = {cap->height * 0.5 * is_hori, cap->height * 0.5 * (1 - is_hori)};

    glm_vec2_rotate(cap_p1, collider2->parent->tran->euler_angle[2], cap_p1);
    glm_vec2_rotate(cap_p2, collider2->parent->tran->euler_angle[2], cap_p2);

    glm_vec2_add(cap_p1, p2, cap_p1);
    glm_vec2_add(cap_p2, p2, cap_p2);

    vec2 box_p[] = {
        {  box->size[0],  box->size[1], },
        { -box->size[0],  box->size[1], },
        { -box->size[0], -box->size[1], },
        {  box->size[0], -box->size[1] }
    };

    for (int i = 0; i < 4; i++) {
        glm_vec2_rotate(box_p[i], collider1->parent->tran->euler_angle[2], box_p[i]);
        glm_vec2_add(p1, box_p[i], box_p[i]);
    }

    collision2d_info info = { .points_count = 0 };
    for (i32 i = 0; i < 4; i++) {
        vec2 cp;
        find_closest_point_on_line(cp, cap_p1, cap_p2, box_p[i]);
        f32 d = cap->radius - glm_vec2_distance(cp, box_p[i]);
        if (d > 0) {
            vec2 normal;
            glm_vec2_sub(cp, box_p[i], normal);
            glm_vec2_normalize_to(normal, info.collision_points[info.points_count].normal);
            info.collision_points[info.points_count].depth = d;
            glm_vec2_mulsubs(info.collision_points[info.points_count].normal, cap->radius, cp);
            glm_vec2_copy(cp, info.collision_points[info.points_count].contact);
            info.points_count++;
        }
    }
    if (info.points_count == 2) {
        return info;
    }

    f32 min_dis = FLT_MAX;
    vec2 contact_point;

    for (int i = 0; i < 4; ++i) {
        vec2 cp;
        find_closest_point_on_line(cp, box_p[i], box_p[(i + 1) % 4], cap_p1);
        f32 dis = glm_vec2_distance(cp, cap_p1);
        if (dis < min_dis) {
            glm_vec2_copy(cp, contact_point);
            min_dis = dis;
        }
    }

    vec2 normal;
    glm_vec2_sub(contact_point, cap_p1, normal);
    glm_vec2_normalize(normal);
    f32 depth = cap->radius - min_dis;

    if (depth > 0) {
        info.collision_points[info.points_count++] = (collision2d_point) {
            .depth = depth,
            .normal = {-normal[0], -normal[1]},
            .contact = {cap_p1[0] + normal[0] * cap->radius, cap_p1[1] + normal[1] * cap->radius},
        };
    }

    if (info.points_count == 2) {
        return info;
    }

    min_dis = FLT_MAX;

    for (int i = 0; i < 4; ++i) {
        vec2 cp;
        find_closest_point_on_line(cp, box_p[i], box_p[(i + 1) % 4], cap_p2);
        f32 dis = glm_vec2_distance(cp, cap_p2);
        if (dis < min_dis) {
            glm_vec2_copy(cp, contact_point);
            min_dis = dis;
        }
    }

    glm_vec2_sub(contact_point, cap_p2, normal);
    glm_vec2_normalize(normal);
    depth = cap->radius - min_dis;

    if (depth > 0) {
        info.collision_points[info.points_count++] = (collision2d_point) {
            .depth = depth,
            .normal = {-normal[0], -normal[1]},
            .contact = {cap_p2[0] + normal[0] * cap->radius, cap_p2[1] + normal[1] * cap->radius},
        };
    }

    return info;
}

collision2d_info capsule2d_box2d_collision_impl(collider2d* collider1, collider2d* collider2) {
    collision2d_info info = box2d_capsule2d_collision_impl(collider2, collider1);
    for (i32 i = 0; i < info.points_count; i++) {
        info.collision_points[i].normal[0] *= -1;
        info.collision_points[i].normal[1] *= -1;
    }
    return info;
}

