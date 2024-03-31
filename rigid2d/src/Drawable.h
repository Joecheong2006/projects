#pragma once
#include "glm/glm.hpp"

namespace mfw {
    class Renderer;
};

struct Drawable {
    bool drawEnable = true;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) = 0;
};

