#pragma once
#include "OdeSolver.h"

struct EulerOdeSolver : OdeSolver {
    virtual void solve(const f64& dt, const std::vector<RigidBody*>& objects) override;
};
