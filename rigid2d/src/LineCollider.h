#pragma once
#include "Collider.h"

struct LineCollider : Collider {
    virtual CollisionState testCollision(
            const Collider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
    virtual CollisionState testCollision(
            const CircleCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
    virtual CollisionState testCollision(
            const LineCollider* collider, 
            const RigidBody* object1, 
            const RigidBody* object2) const override;
};

