#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"
#include "util.h"
#include "mfwpch.h"

namespace mfw {
    class Renderer;
};

class PointConstraint: public Constraint {
public:
    GENERATE_OBJECT_IDENTIFIER();
    PointConstraint();

    f32 d;
    RigidBody* target;
    glm::dvec2 m_pos;
    glm::dvec3 m_color;
    
};

