#pragma once
#include "Collider2D.h"

struct CircleCollider2D : Collider2D {
    real radius;
    vec2 offset;
    virtual AABB getBounds() const override;
    inline virtual CollisionState testCollision(
            const Collider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const override;
    inline virtual CollisionState testCollision(
            const CircleCollider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const override;
    inline virtual CollisionState testCollision(
            const CylinderCollider2D* collider, 
            const RigidBody2D* object1, 
            const RigidBody2D* object2) const override;
};

