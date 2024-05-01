#include "CircleCollider.h"
#include "Collision.h"
#include "Circle.h"

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
    return Collision::TestCircleToCircle(object1, object2);
}

CollisionState CircleCollider::testCollision(
        [[maybe_unused]] const LineCollider* collider, 
        const RigidBody* object1, 
        const RigidBody* object2) const
{
    return Collision::TestCircleToLine(object2, object1);
}

