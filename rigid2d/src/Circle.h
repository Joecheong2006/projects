#pragma once

#include "Object.h"

namespace mfw {
    class Renderer;
};

class Circle : public Object {
public:
    SET_OBJECT_NORMAL_BEHAVIOUR(ObjectType::Circle);

    f32 r;
    Circle();
    Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d);

    void render(const glm::mat4& proj, mfw::Renderer& renderer) const;

};

