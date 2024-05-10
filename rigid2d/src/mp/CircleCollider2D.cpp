#include "CircleCollider2D.h"
#include "TestCollision2D.h"
#include "Circle.h"
#include "Cylinder.h"

AABB CircleCollider2D::getBounds() const {
    return AABB(vec2(radius) * 2.0, owner->m_position + offset);
}

CollisionState CircleCollider2D::testCollision(
        const Collider2D* collider, 
        const RigidBody2D* object1, 
        const RigidBody2D* object2) const
{
    return collider->testCollision(this, object1, object2);
}

CollisionState CircleCollider2D::testCollision(
        [[maybe_unused]] const CircleCollider2D* collider, 
        const RigidBody2D* object1, 
        const RigidBody2D* object2) const
{

    return TestCollision2D::CircleToCircle(
            static_cast<const CircleCollider2D*>(&object2->getCollider()),
            static_cast<const CircleCollider2D*>(&object1->getCollider()));
}

CollisionState CircleCollider2D::testCollision(
        [[maybe_unused]] const CylinderCollider2D* collider, 
        const RigidBody2D* object1, 
        const RigidBody2D* object2) const
{
    return TestCollision2D::CircleToCylinder(
            static_cast<const CircleCollider2D*>(&object2->getCollider()),
            static_cast<const CylinderCollider2D*>(&object1->getCollider()));
}

