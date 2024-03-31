#include "PointConstraint.h"
#include "PhysicsEmulator.h"

PointConstraint::PointConstraint()
    : d(PhysicsEmulator::sim->attri.node_size * 1.7), target(nullptr)
{}

