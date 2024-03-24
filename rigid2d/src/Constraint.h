#pragma once
#include "util.h"
#include "glm/glm.hpp"

#define SET_CONSTRAINT_NORMAL_BEHAVIOUR(type) \
    static ConstraintType GetType() {\
        return type;\
    }\
    virtual inline ConstraintType getType() override {\
        return type;\
    }

enum class ConstraintType {
    Distance,
    Point,
};

namespace mfw {
    class Renderer;
};

class Constraint {
public:
    Constraint() {}
    virtual ~Constraint() {}

    virtual inline ConstraintType getType() = 0;
    virtual void solve(f64 dt) = 0;
    virtual void render(const glm::mat4& proj, mfw::Renderer& renderer) = 0;

};

