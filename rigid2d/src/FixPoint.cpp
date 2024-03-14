#include "FixPoint.h"
#include "Stick.h"

FixPoint::FixPoint(f32 r, const glm::vec2& pos)
    : Object2D(pos, 0, glm::vec4(1)), holding(nullptr), r(r * 0.1)
{}

void FixPoint::fix() {
    if (holding) {
        holding->m_pos = m_pos;
    }
}

void FixPoint::render(const glm::mat4& o) {
    Stick::renderer->draw(o, m_pos - glm::dvec2(r, 0), m_pos + glm::dvec2(r, 0), glm::vec4(COLOR(0xaf7434), 1), r);
}

