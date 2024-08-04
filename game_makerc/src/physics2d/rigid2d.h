#ifndef _PHYSICS_RIGID2D_
#define _PHYSICS_RIGID2D_
#include "core/util.h"
#include "transform.h"

typedef struct rigid2d rigid2d;
typedef void(*rigid2d_process_callback)(rigid2d*,f32);
typedef struct collider2d collider2d;

struct rigid2d {
    transform* tran;
    collider2d* collider;
    vec2 a, v, g;
    f32 angular_v, angular_drag;
    f32 mass, inverse_mass, restitution, inertia, inverse_inertia, drag;
    i32 is_static, freeze_rotation;
    i32 index;
    rigid2d_process_callback process;
};

void euler_method(rigid2d* rig, f32 dt);
void verlet_method(rigid2d* rig, f32 dt);

void init_rigid2d(rigid2d* rig, transform* tran);
void rigid2d_set_mass(rigid2d* rig, f32 new_mass);
void rigid2d_set_static(rigid2d* rig);
void rigid2d_update_inertia(rigid2d* rig, collider2d* collider);

#endif
