#pragma once
#include "util.h"

struct CollisionState {
    vec2 normal, contact;
    real depth;
};

class RigidBody;
struct CircleCollider;
struct LineCollider;
struct Collider {
    virtual ~Collider() = default;
    virtual CollisionState testCollision(
            const Collider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const = 0;
    virtual CollisionState testCollision(
            const CircleCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const = 0;
    virtual CollisionState testCollision(
            const LineCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const = 0;
};

