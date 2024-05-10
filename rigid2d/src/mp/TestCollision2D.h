#pragma once
#include "Collider2D.h"

namespace TestCollision2D {
    CollisionState CircleToCircle(const CircleCollider2D* circleCollider1, const CircleCollider2D* circleCollider2);
    CollisionState CircleToCylinder(const CircleCollider2D* circleCollider, const CylinderCollider2D* cylinderCollider);
    bool AABB(const AABB& aabb1, const AABB& aabb2);

};
