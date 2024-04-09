#pragma once
#include "glm/glm.hpp"

namespace mfw {
    class Renderer;
};

class Drawable {
private:
    friend class World;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) = 0;

public:
    bool drawEnable = true;

};

