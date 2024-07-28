#ifndef _PHYSICS_RIGID2D_
#define _PHYSICS_RIGID2D_
#include "../util.h"
#include "../transform.h"

typedef struct rigid2d rigid2d;
typedef void(*rigid2d_process_callback)(rigid2d*,float);

struct rigid2d {
    transform* tran;
    vec2 a, v, g;
    f32 mass, inverse_mass, restitution;
    i32 is_static;
    rigid2d_process_callback process;
};

void euler_method(rigid2d* rig, float dt);
void rigid2d_set_mass(rigid2d* rig, f32 new_mass);
void init_rigid2d(rigid2d* rig, transform* tran);
void rigi2d_set_static(rigid2d* rig);

#endif
