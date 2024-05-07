#pragma once
#include "Collider.h"

struct CircleCollider : Collider {
    virtual CollisionState testCollision(
            const Collider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
    virtual CollisionState testCollision(
            const CircleCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
    virtual CollisionState testCollision(
            const CylinderCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
};

