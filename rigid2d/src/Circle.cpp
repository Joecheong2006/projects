#include "Circle.h"
#include "Renderer.h"

Circle::Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d)
    : Object(pos, d, color), r(d)
{}

Circle::Circle()
    : Object(glm::vec2(0), 1, glm::vec4(1)), r(1)
{}

void Circle::render(const glm::mat4& proj, mfw::Renderer& renderer) const {
    renderer.renderCircle(proj, this);
}

