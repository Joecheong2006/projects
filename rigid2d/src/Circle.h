#pragma once
#include "Object.h"

class Circle : public Object {
public:
    GENERATE_OBJECT_IDENTIFIER();

    f32 r;
    Circle();
    Circle(const glm::vec2& pos, const glm::vec3& color, const f32& r);

    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

};

