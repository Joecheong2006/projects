#pragma once
#include "OdeSolver.h"

struct VerletOdeSolver : Solver {
    virtual void solve(const real& dt, const std::vector<RigidBody2D*>& objects) override;
};
