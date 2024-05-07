#pragma once
#include "OdeSolver.h"

struct VerletOdeSolver : OdeSolver {
    virtual void solve(const real& dt, const std::vector<RigidBody*>& objects) override;
};
