#include "LineCollider.h"
#include "Collision.h"
#include "Circle.h"
#include "Line.h"

CollisionState LineCollider::testCollision(
        const Collider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return collider->testCollision(this, object1, object2);
}

CollisionState LineCollider::testCollision(
        [[maybe_unused]] const CircleCollider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return Collision::TestCircleToLine(object1, object2);
}

CollisionState LineCollider::testCollision(
        [[maybe_unused]] const LineCollider* collider, 
        [[maybe_unused]] const RigidBody* object1, 
        [[maybe_unused]] const RigidBody* object2) const
{
    return {};
}

