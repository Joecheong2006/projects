#pragma once
#include "util.h"

namespace mfw {
    class Renderer;
};

class Drawable {
private:
    friend class PhysicsWorld;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) = 0;

public:
    bool drawEnable = true;

};

