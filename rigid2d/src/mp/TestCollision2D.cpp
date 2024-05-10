#include "TestCollision2D.h"
#include "Circle.h"
#include "Cylinder.h"
#include "AABB.h"

namespace TestCollision2D {
    CollisionState CircleToCircle(const CircleCollider2D* circleCollider1, const CircleCollider2D* circleCollider2) {
        auto pos1 = circleCollider1->owner->m_position + circleCollider1->offset;
        auto pos2 = circleCollider2->owner->m_position + circleCollider2->offset;

        const real distance = glm::length(pos1 - pos2);
        const vec2 normal = (pos1 - pos2) / distance;
        const real depth = distance - (circleCollider1->radius + circleCollider2->radius);
        return {
            -normal, -depth * normal, depth,
        };
    }

    CollisionState CircleToCylinder(const CircleCollider2D* circleCollider, const CylinderCollider2D* cylinderCollider) {
        auto circle_pos = circleCollider->owner->m_position + circleCollider->offset;
        auto cylinder = static_cast<const Cylinder*>(cylinderCollider->owner);
        auto cylinder_p1 = cylinder->p[0] + cylinderCollider->offset;
        auto cylinder_p2 = cylinder->p[1] + cylinderCollider->offset;

        const vec2 direction = glm::normalize(cylinder_p2 - cylinder_p1);
        const vec2 contact = glm::clamp(
                glm::dot(circle_pos - cylinder_p1, direction), 0.0, glm::length(cylinder_p2 - cylinder_p1)
                ) * direction + cylinder_p1;

        return {
            -glm::normalize(circle_pos - contact), contact,
            glm::length(circle_pos - contact) - cylinderCollider->size.y - circleCollider->radius,
        };
    }

    bool AABB(const struct AABB& aabb1, const struct AABB& aabb2) {
        return (aabb1.max.x - aabb2.min.x > 0 &&
                aabb1.max.y - aabb2.min.y > 0 &&
                aabb1.min.x - aabb2.max.x < 0 &&
                aabb1.min.y - aabb2.max.y < 0);
    }

};
