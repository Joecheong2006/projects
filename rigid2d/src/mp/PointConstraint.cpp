#include "PointConstraint.h"

PointConstraint::PointConstraint()
    : d(), target(nullptr)
{}

void PointConstraint::setTarget(RigidBody2D* body) {
    target = body;
    target->setStatic();
}

void PointConstraint::releaseTarget() {
    target->setDynamic();
    target = nullptr;
}

