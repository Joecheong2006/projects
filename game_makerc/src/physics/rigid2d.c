#include "rigid2d.h"
#include <string.h>

void euler_method(rigid2d* rig, float dt) {
    ASSERT(rig->tran != NULL);
    rig->a[0] += rig->g[0];
    rig->a[1] += rig->g[1];

    rig->v[0] += rig->a[0] * dt;
    rig->v[1] += rig->a[1] * dt;

    rig->tran->position[0] += rig->v[0] * dt;
    rig->tran->position[1] += rig->v[1] * dt;

    rig->a[0] = 0;
    rig->a[1] = 0;
}

void rigid2d_set_mass(rigid2d* rig, f32 new_mass) {
    ASSERT(new_mass > 0);
    rig->mass = new_mass;
    rig->inverse_mass = 1.0 / new_mass;
}

void init_rigid2d(rigid2d* rig, transform* tran) {
    memset(rig, 0, sizeof(rigid2d));
    rigid2d_set_mass(rig, 1);
    rig->tran = tran;
    rig->g[1] = -9.81;
    rig->process = euler_method;
    rig->restitution = 1;
}

void rigi2d_set_static(rigid2d* rig) {
    rig->inverse_mass = 0;
    rig->is_static = 1;
}