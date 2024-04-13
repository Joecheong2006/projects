#pragma once
#include "util.h"
#include <mfw/mfwpch.h>

class RigidBody;
struct OdeSolver {
    virtual void solve(const f64& dt, const std::vector<RigidBody*>& objects) = 0;
};

