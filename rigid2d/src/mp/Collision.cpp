#include "Collision.h"
#include "Circle.h"
#include "Cylinder.h"

namespace Collision {
    CollisionState TestCircleToCircle(const RigidBody* object1, const RigidBody* object2) {
        auto circle1 = static_cast<const Circle*>(object1);
        auto circle2 = static_cast<const Circle*>(object2);

        const real distance = glm::length(circle1->m_position - circle2->m_position);
        const vec2 normal = (circle1->m_position - circle2->m_position) / distance;
        const real depth = distance - (circle1->radius + circle2->radius);
        return {
            -normal, -depth * normal, depth,
        };
    }

    CollisionState TestCircleToLine(const RigidBody* object1, const RigidBody* object2) {
        auto circle = static_cast<const Circle*>(object1);
        auto line = static_cast<const Cylinder*>(object2);
        
        const vec2 direction = glm::normalize(line->p[1] - line->p[0]);
        const vec2 contact = glm::clamp(
                glm::dot(circle->m_position - line->p[0], direction), 0.0, glm::length(line->p[1] - line->p[0])
                ) * direction + line->p[0];

        return {
            -glm::normalize(circle->m_position - contact), contact,
            glm::length(circle->m_position - contact) - line->width - circle->radius,
        };
    }

};
