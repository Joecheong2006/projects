#pragma once

#include "util.h"
#include "glm/glm.hpp"

namespace mfw {
    class FixPoint {
    public:
        glm::vec2* holding;
        glm::vec2 pos;
        f32 r = 0.3f;

        explicit FixPoint(const glm::vec2& pos = glm::vec2(0));

        void fix();
        void render(glm::mat4& o);

    };

}

