#include "CylinderCollider.h"
#include "TestCollision2D.h"
#include "Circle.h"
#include "Cylinder.h"
#include <mfw/mfwlog.h>

AABB CylinderCollider2D::getBounds() const {
    auto cylinder = static_cast<const Cylinder*>(owner);
    auto direction = glm::normalize(cylinder->p[0] - cylinder->p[1]);
    return AABB(vec2(abs(cylinder->p[0] - cylinder->p[1])) + cylinder->width * direction * 2.0, cylinder->m_position + offset);
}

CollisionState CylinderCollider2D::testCollision(
        const Collider2D* collider, 
        const RigidBody2D* object1, 
        const RigidBody2D* object2) const
{
    return collider->testCollision(this, object1, object2);
}

CollisionState CylinderCollider2D::testCollision(
        [[maybe_unused]] const CircleCollider2D* collider, 
        const RigidBody2D* object1, 
        const RigidBody2D* object2) const
{

    auto state = TestCollision2D::CircleToCylinder(
            static_cast<const CircleCollider2D*>(&object1->getCollider()),
            static_cast<const CylinderCollider2D*>(&object2->getCollider()));
    state.normal = -state.normal;
    return state;
}

CollisionState CylinderCollider2D::testCollision(
        [[maybe_unused]] const CylinderCollider2D* collider, 
        [[maybe_unused]] const RigidBody2D* object1, 
        [[maybe_unused]] const RigidBody2D* object2) const
{
    return {};
}

