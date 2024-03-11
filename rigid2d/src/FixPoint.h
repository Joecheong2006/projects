#pragma once

#include "util.h"
#include "glm/glm.hpp"
#include "Object.h"

namespace mfw {
    class FixPoint : public Object2D {
    public:
        Object2D* holding;
        f32 r = 0.6;

        explicit FixPoint(const glm::vec2& pos = glm::vec2(0));

        void fix();
        void render(glm::mat4& o);

    private:
        void update(const f64& frame) override { (void)frame; }

    };

}

