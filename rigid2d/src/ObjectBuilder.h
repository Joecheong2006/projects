#pragma once

#include "World.h"
#include "Simulation.h"
#include "Circle.h"
#include "DistanceConstraint.h"
#include "Tracer.h"
#include "Rotator.h"
#include "FixPoint.h"

template <typename T>
struct ObjectBuilder {};

template <>
struct ObjectBuilder<Circle> {
    const glm::vec3 default_color;
    const f32 default_d;
    inline Circle* operator()(const glm::vec2& pos, const glm::vec3& color, const f32& d) {
        static const f32 worldScale = Simulation::Get()->getWorldScale();
        auto circle = Simulation::Get()->world.addObject<Circle>(pos * worldScale, color, d * worldScale);
        return circle;
    }
    inline Circle* operator()(const glm::vec2& pos, const f32& d) {
        return (*this)(pos, default_color, d);
    }
    inline Circle* operator()(const glm::vec2& pos) {
        return (*this)(pos, default_color, default_d);
    }
    inline Circle* operator()() {
        return (*this)({}, default_color, default_d);
    }
};

template <>
struct ObjectBuilder<DistanceConstraint> {
    const glm::vec3 default_color;
    const f32 default_w;
    inline DistanceConstraint* operator()(Object* t1, Object* t2, f32 d, f32 w, glm::vec3 color) {
        static const f32 worldScale = Simulation::Get()->getWorldScale();
        auto distanceConstraint = Simulation::Get()->world.addConstraint<DistanceConstraint>(t1, t2, d * worldScale, w * worldScale);
        distanceConstraint->color = color;
        return distanceConstraint;
    }
    inline DistanceConstraint* operator()(Object* t1, Object* t2, f32 d) {
        return (*this)(t1, t2, d, default_w, default_color);
    }
};

template <>
struct ObjectBuilder<Tracer> {
    const glm::vec3 default_color;
    const f32 default_maxScale, default_minScale, default_dr;
    const i32 default_maxSamples;
    inline Tracer* operator()(Object* target, f32 maxScale, f32 minScale, f32 dr, i32 maxSamples, glm::vec3 color) {
        static const f32 worldScale = Simulation::Get()->getWorldScale();
        auto tracer = Simulation::Get()->world.addObject<Tracer>(target);
        tracer->maxScale = maxScale * worldScale;
        tracer->minScale = minScale * worldScale;
        tracer->maxSamples = maxSamples;
        tracer->dr = dr;
        tracer->m_color = color;
        return tracer;
    }
    inline Tracer* operator()(Object* target) {
        return (*this)(target, default_maxScale, default_minScale, default_dr, default_maxSamples, default_color);
    }
};

template <>
struct ObjectBuilder<Rotator> {
    const f32 default_r, default_w;
    inline Rotator* operator()(Object* center, Object* target, f32 r, f32 w) {
        static const f32 worldScale = Simulation::Get()->getWorldScale();
        auto rotator = Simulation::Get()->world.addConstraint<Rotator>(center, target);
        rotator->r = r * worldScale;
        rotator->w = w;
        return rotator;
    }
    inline Rotator* operator()(Object* center, Object* target) {
        return (*this)(center, target, default_r, default_w);
    }
};

template <>
struct ObjectBuilder<FixPoint> {
    const glm::vec3 default_color;
    const f32 default_d;
    inline FixPoint* operator()(glm::vec2 pos, f32 d, glm::vec3 color) {
        static const f32 worldScale = Simulation::Get()->getWorldScale();
        auto fixPoint = Simulation::Get()->world.addConstraint<FixPoint>();
        fixPoint->self = Object(pos, 0, color);
        fixPoint->d = d * worldScale;
        return fixPoint;
    }
    inline FixPoint* operator()(glm::vec2 pos) {
        return (*this)(pos, default_d, default_color);
    }
};

