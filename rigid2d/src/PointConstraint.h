#pragma once

#include "Constraint.h"
#include "Object.h"
#include "util.h"
#include "mfwpch.h"

namespace mfw {
    class Renderer;
};

class PointConstraint: public Constraint {
public:
    GENERATE_CONSTRAINT_IDENTIFIER(PointConstraint);
    PointConstraint();

    f32 d;
    Object* target;
    Object self;
    
};

