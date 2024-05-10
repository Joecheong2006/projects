#include <mfw/mfwpch.h>
#pragma once
#include "util.h"

class RigidBody2D;
struct Solver {
    virtual void solve(const f64& dt, const std::vector<RigidBody2D*>& objects) = 0;
};

