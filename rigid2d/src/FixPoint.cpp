#include "FixPoint.h"
#include "Stick.h"

namespace mfw {
    FixPoint::FixPoint(const glm::vec2& pos)
        : holding(nullptr), pos(pos)
    {}

    void FixPoint::fix() {
        if (holding) {
            *holding = pos;
        }
    }

    void FixPoint::render(glm::mat4& o) {
        Stick::renderer->draw(o, pos - glm::vec2(r, 0), pos + glm::vec2(r, 0), glm::vec3(COLOR(0xaf7434)), r);
    }
}
