#include "CylinderCollider.h"
#include "Collision.h"
#include <mfw/mfwlog.h>

CollisionState CylinderCollider::testCollision(
        const Collider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return collider->testCollision(this, object1, object2);
}

CollisionState CylinderCollider::testCollision(
        [[maybe_unused]] const CircleCollider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    auto state = Collision::TestCircleToLine(object1, object2);
    state.normal = -state.normal;
    return state;
    return Collision::TestCircleToLine(object1, object2);
}

CollisionState CylinderCollider::testCollision(
        [[maybe_unused]] const CylinderCollider* collider, 
        [[maybe_unused]] const RigidBody* object1, 
        [[maybe_unused]] const RigidBody* object2) const
{
    return {};
}

