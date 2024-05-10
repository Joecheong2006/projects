#pragma once
#include "util.h"
#include "AABB.h"

struct CollisionState {
    vec2 normal, contact;
    real depth;
};

class RigidBody2D;
struct CircleCollider2D;
struct CylinderCollider2D;
struct Collider2D {
    const RigidBody2D * owner;
    bool enable = true;
    virtual AABB getBounds() const = 0;
    virtual CollisionState testCollision(
            const Collider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const = 0;
    virtual CollisionState testCollision(
            const CircleCollider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const = 0;
    virtual CollisionState testCollision(
            const CylinderCollider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const = 0;
};

