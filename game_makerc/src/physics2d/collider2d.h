#ifndef _PHYSICS_COLLIDER2D_
#define _PHYSICS_COLLIDER2D_
#include "rigid2d.h"
#include <cglm/cglm.h>

typedef enum {
    ColliderBox2d = 0,
    ColliderCircle2d,
    ColliderCylinder2d,
} Collider2dType;

typedef struct {
    vec2 normal, contact;
    f32 depth;
} collision2d_point;

typedef struct {
    collision2d_point collision_points[4];
    i32 points_count;
} collision2d_info;

typedef struct collider2d collider2d;
typedef collision2d_info(*collider2d_collision_callback)(collider2d*, collider2d*);
typedef f32(*collider2d_get_inertia_callback)(collider2d*);

struct collider2d {
    Collider2dType type;
    collider2d_collision_callback collide;
    collider2d_get_inertia_callback get_inertia;
    void* self;
    rigid2d* parent;
};

collider2d create_collider2d(Collider2dType type, rigid2d* parent, void* context);

#endif
