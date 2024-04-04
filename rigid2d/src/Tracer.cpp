#include "Tracer.h"
#include "Renderer.h"
#include "Circle.h"

Tracer::Tracer(Object* target): target(target)
{}

void Tracer::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    if (!target)
        return;

    if ((i32)positions_trace.size() == maxSamples) {
        positions_trace.pop_front();
    }
    positions_trace.push_back(target->m_pos);

    f32 i = 0;
    for (auto iter = positions_trace.begin();;) {
        const glm::vec2 p1 = *(iter);
        if (++iter == positions_trace.end())
            break;
        const glm::vec2 p2 = *iter;
        const glm::vec3 trace = glm::vec3(COLOR(0xc73e3e)), background = glm::vec3(COLOR(0x191919));
        const glm::vec3 color = (trace - background) * (i++ / positions_trace.size()) + background;
        f32 t = maxScale * (i / positions_trace.size());
        t = glm::clamp(t - maxScale * dr, minScale, maxScale);
        renderer.renderCircleI(proj, { p2, color, t });
        renderer.renderCircleI(proj, { p1, color, t });
        renderer.renderLineI(proj, p1, p2, color, t);
    }
}
