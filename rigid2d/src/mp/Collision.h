#pragma once
#include "Collider.h"

class RigidBody;
namespace Collision {
    CollisionState TestCircleToCircle(const RigidBody* object1, const RigidBody* object2);
    CollisionState TestCircleToLine(const RigidBody* object1, const RigidBody* object2);

};
