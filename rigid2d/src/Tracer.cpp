#include "Tracer.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"

Tracer::Tracer(RigidBody* target)
    : target(target)
{}

void Tracer::draw(const mat4& proj, mfw::Renderer& renderer) {
    if (!target)
        return;

    if ((i32)positions_trace.size() == maxSamples) {
        positions_trace.pop_front();
    }
    positions_trace.push_back(target->m_position);

    f32 i = 0;
    for (auto iter = positions_trace.begin();;) {
        const glm::vec2 p1 = *(iter);
        if (++iter == positions_trace.end())
            break;
        const glm::vec2 p2 = *iter;
        // const glm::vec3 trace = glm::vec3(COLOR(0xc73e3e)), background = glm::vec3(COLOR(0x191919));
        const glm::vec3 background = glm::vec3(COLOR(0x191919));
        const glm::vec3 color = (m_color - background) * (i++ / positions_trace.size()) + background;
        f32 t = maxScale * (i / positions_trace.size());
        t = glm::clamp(t - maxScale * dr, minScale, maxScale);
        renderer.renderCircleI(proj, { p2, color, t });
        renderer.renderCircleI(proj, { p1, color, t });
        renderer.renderLineI(proj, p1, p2, color, t);
    }
}

color ObjectBuilder<Tracer>::default_color;
real ObjectBuilder<Tracer>::default_maxScale;
real ObjectBuilder<Tracer>::default_minScale;
real ObjectBuilder<Tracer>::default_dr;
i32 ObjectBuilder<Tracer>::default_maxSamples;

Tracer* ObjectBuilder<Tracer>::operator()(RigidBody* target, real maxScale, real minScale, real dr, i32 maxSamples, color color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    auto tracer = Simulation::Get()->world.addConstraint<Tracer>(target);
    tracer->maxScale = maxScale * worldScale;
    tracer->minScale = minScale * worldScale;
    tracer->maxSamples = maxSamples;
    tracer->dr = dr;
    tracer->m_color = color;
    return tracer;
}

