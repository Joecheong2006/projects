#include "FixPoint.h"
#include "Stick.h"

namespace mfw {
    FixPoint::FixPoint(const glm::vec2& pos)
        : Object2D(pos, 0, glm::vec4(1)), holding(nullptr)
    {}

    void FixPoint::fix() {
        if (holding) {
            holding->m_pos = m_pos;
        }
    }

    void FixPoint::render(glm::mat4& o) {
        Stick::renderer->draw(o, m_pos - glm::vec2(r, 0), m_pos + glm::vec2(r, 0), glm::vec4(COLOR(0xaf7434), 1), r);
    }
}
