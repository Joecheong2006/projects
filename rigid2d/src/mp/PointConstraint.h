#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"
#include "util.h"

namespace mfw {
    class Renderer;
};

class PointConstraint: public Constraint {
public:
    GENERATE_OBJECT_IDENTIFIER();
    PointConstraint();

    real d;
    vec2 m_position;
    color m_color;
    RigidBody* target;
    void setTarget(RigidBody* body);
    void releaseTarget();
    
};

