#pragma once
#include "util.h"

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

class Constraint {
public:
    Constraint() {}
    virtual ~Constraint() {}

    virtual inline ConstraintType getType() = 0;
    virtual void solve(f64 dt) = 0;

};

