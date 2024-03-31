#include "PointConstraint.h"
#include "Simulation.h"

PointConstraint::PointConstraint()
    : d(Simulation::Get()->attri.node_size * 1.7), target(nullptr)
{}

