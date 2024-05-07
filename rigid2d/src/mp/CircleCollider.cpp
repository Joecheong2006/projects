#include "CircleCollider.h"
#include "Collision.h"
#include <mfw/mfwlog.h>

CollisionState CircleCollider::testCollision(
        const Collider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return collider->testCollision(this, object1, object2);
}

CollisionState CircleCollider::testCollision(
        [[maybe_unused]] const CircleCollider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return Collision::TestCircleToCircle(object2, object1);
}

CollisionState CircleCollider::testCollision(
        [[maybe_unused]] const CylinderCollider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return Collision::TestCircleToLine(object2, object1);
}

